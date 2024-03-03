#include "Reliquary.h"

#include <unordered_set>

#include "RelicParented.h"

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>

#include <utility>
#include "Chroma/StringUtility.h"

using namespace std::string_literals;

namespace Arca
{
    void Reliquary::Work()
    {
        WorkCurators([](Curator& curator) { curator.StartStep(); });
        WorkCurators([](Curator& curator) { curator.Work(); });
        WorkCurators([](Curator& curator) { curator.StopStep(); });

        for(auto& signalBatchSource : signalBatchSources)
            signalBatchSource.second->Clear();
    }

    void Reliquary::Destroy(const RelicHandle& handle)
    {
        if (&handle.Owner() != this)
            return;

        const auto metadata = RelicMetadataFor(handle.ID());
        if (!WillDestroyRelic(metadata))
            return;

        DestroyRelic(*metadata);
    }

    void Reliquary::ParentRelic(const RelicHandle& parent, const RelicHandle& child)
    {
        const auto parentID = parent.ID();
        const auto childID = child.ID();

        if (parent == child)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(parentID) + ") was attempted to be parented to itself.");

        if (&parent.Owner() != this)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(parentID) + ") is from a different Reliquary.");

        if (&child.Owner() != this)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(childID) + ") is from a different Reliquary.");

        auto parentMetadata = RelicMetadataFor(parentID);
        if (!parentMetadata)
            throw CannotFindRelic(parentID);

        if (parentMetadata->dynamism == RelicDynamism::Static)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(childID) + ") was attempted to be parented to a static relic.");

        auto childMetadata = RelicMetadataFor(childID);
        if (!childMetadata)
            throw CannotFindRelic(childID);

        if (childMetadata->dynamism == RelicDynamism::Static)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(childID) + ") is static and cannot be parented to anything.");

        if (childMetadata->parent.has_value())
            throw CannotParentRelic(
                "The relic with id("s + Chroma::ToString(childID) + ") is already parented.");;

        parentMetadata->children.push_back(childID);
        childMetadata->parent = parentID;

        SignalRelicParented(parent, child);
    }

    Reliquary::SizeT Reliquary::RelicSize() const
    {
        return relicMetadataList.size();
    }

    Reliquary::KnownPolymorphicSerializer::KnownPolymorphicSerializer(
        TypeHandle mainTypeHandle,
        Serializer&& serializer,
        InscriptionTypeHandleProvider&& inscriptionTypeProvider)
        :
        mainTypeHandle(std::move(mainTypeHandle)),
        serializer(std::move(serializer)),
        inscriptionTypeProvider(std::move(inscriptionTypeProvider))
    {}

    void Reliquary::SetupNewRelicInternals(
        RelicID id,
        RelicDynamism dynamism,
        std::optional<TypeHandle> typeHandle,
        void* storage)
    {
        occupiedRelicIDs.Include(id);
        relicMetadataList.push_back({ id, dynamism, std::move(typeHandle), storage });
    }

    void Reliquary::DestroyRelicMetadata(RelicID id)
    {
        occupiedRelicIDs.Remove(id);
        const auto itr = std::remove_if(
            relicMetadataList.begin(),
            relicMetadataList.end(),
            [id](const RelicMetadata& metadata) { return metadata.id == id; });
        if (itr == relicMetadataList.end())
            return;
        relicMetadataList.erase(itr);
    }

    auto Reliquary::RelicMetadataFor(RelicID id) -> RelicMetadata*
    {
        const auto found = std::find_if(
            relicMetadataList.begin(),
            relicMetadataList.end(),
            [id](const RelicMetadata& metadata) { return metadata.id == id; });
        return found != relicMetadataList.end() ? &*found : nullptr;
    }

    auto Reliquary::RelicMetadataFor(RelicID id) const -> const RelicMetadata*
    {
        const auto found = std::find_if(
            relicMetadataList.begin(),
            relicMetadataList.end(),
            [id](const RelicMetadata& metadata) { return metadata.id == id; });
        return found != relicMetadataList.end() ? &*found : nullptr;
    }

    void Reliquary::SatisfyRelicStructure(const RelicStructure& structure, RelicID id)
    {
        for(auto& entry : structure)
            CreateShard(entry.typeHandle, id, entry.isConst);
    }

    bool Reliquary::WillDestroyRelic(RelicMetadata* metadata) const
    {
        return metadata && metadata->dynamism != RelicDynamism::Static;
    }

    void Reliquary::DestroyRelic(RelicMetadata& metadata)
    {
        auto& id = metadata.id;

        for (auto& child : metadata.children)
            DestroyRelic(*RelicMetadataFor(child));

        for (auto& shardBatchSource : shardBatchSources)
            shardBatchSource.second->DestroyFromBase(id);

        if (metadata.parent)
        {
            const auto parent = *metadata.parent;
            auto parentMetadata = RelicMetadataFor(parent);
            const auto eraseChildrenItr =
                std::remove_if(
                    parentMetadata->children.begin(),
                    parentMetadata->children.end(),
                    [id](const RelicID& childId) { return id == childId; });
            if (eraseChildrenItr != parentMetadata->children.end())
                parentMetadata->children.erase(eraseChildrenItr);
        }

        if (metadata.typeHandle)
        {
            auto batchSource = FindRelicBatchSource(*metadata.typeHandle);
            batchSource->DestroyFromBase(id);
        }

        DestroyRelicMetadata(id);
    }

    RelicID Reliquary::NextRelicID() const
    {
        const auto itr = occupiedRelicIDs.begin();
        return itr == occupiedRelicIDs.end() || itr->Start() > 1
            ? 1
            : (--occupiedRelicIDs.end())->End() + 1;
    }

    void Reliquary::SignalRelicParented(const RelicHandle& parent, const RelicHandle& child)
    {
        const RelicParented signal{ parent, child };
        Raise(signal);
    }

    Reliquary::NamedRelicStructure::NamedRelicStructure(std::string name, Arca::RelicStructure value) :
        name(std::move(name)), value(std::move(value))
    {}

    RelicBatchSourceBase* Reliquary::FindRelicBatchSource(const TypeHandle& typeHandle)
    {
        const auto found = relicBatchSources.find(typeHandle);
        if (found == relicBatchSources.end())
            return nullptr;

        return found->second.get();
    }

    void Reliquary::CreateShard(const TypeHandle& typeHandle, RelicID id, bool isConst)
    {
        const auto factory = shardFactoryMap.find(typeHandle);
        if (factory == shardFactoryMap.end())
            throw NotRegistered("shard", typeHandle);

        factory->second(*this, id, isConst);
    }

    ShardBatchSourceBase* Reliquary::FindShardBatchSource(const TypeHandle& typeHandle)
    {
        const auto found = shardBatchSources.find(typeHandle);
        if (found == shardBatchSources.end())
            return nullptr;

        return found->second.get();
    }

    SignalBatchSourceBase* Reliquary::FindSignalBatchSource(const std::type_index& type)
    {
        const auto found = signalBatchSources.find(type);
        if (found == signalBatchSources.end())
            return nullptr;

        return found->second.get();
    }
}

namespace Inscription
{
    KnownPolymorphic::KnownPolymorphic(void* underlying, Arca::Reliquary& reliquary, Serializer serializer) :
        underlying(underlying), reliquary(&reliquary), serializer(std::move(serializer))
    {}

    void Scribe<KnownPolymorphic, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        object.serializer(object.underlying, *object.reliquary, archive);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        if (archive.IsOutput())
            Save(object, archive);
        else
            Load(object, archive);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Save(ObjectT& object, ArchiveT& archive)
    {
        archive(object.relicMetadataList);

        JumpSaveAll(
            object,
            archive,
            object.shardSerializers,
            object.shardBatchSources,
            [](ObjectT::ShardBatchSourceMap::value_type& entry) -> void* { return entry.second.get(); },
            [](ObjectT::ShardBatchSourceMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.relicSerializers,
            object.relicBatchSources,
            [](ObjectT::RelicBatchSourceMap::value_type& entry) -> void* { return entry.second.get(); },
            [](ObjectT::RelicBatchSourceMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.staticRelicSerializers,
            object.staticRelicMap,
            [](ObjectT::StaticRelicMap::value_type& entry) -> void* { return &entry.second; },
            [](ObjectT::StaticRelicMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.curatorSerializers,
            object.curators,
            [](ObjectT::CuratorMap::value_type& entry) -> void* { return entry.second->Get(); },
            [](ObjectT::CuratorMap::value_type& entry) { return entry.first; });
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Load(ObjectT& object, ArchiveT& archive)
    {
        archive(object.relicMetadataList);

        JumpLoadAll(
            object,
            archive,
            object.shardSerializers,
            [](ObjectT& object, const ::Arca::TypeHandle& typeHandle)
            {
                return object.FindShardBatchSource(typeHandle);
            });

        JumpLoadAll(
            object,
            archive,
            object.relicSerializers,
            [](ObjectT& object, const ::Arca::TypeHandle& typeHandle)
            {
                return object.FindRelicBatchSource(typeHandle);
            });

        JumpLoadAll(
            object,
            archive,
            object.staticRelicSerializers,
            [](ObjectT& object, const ::Arca::TypeHandle& typeHandle)
            {
                return &object.staticRelicMap.find(typeHandle)->second;
            });

        JumpLoadAll(
            object,
            archive,
            object.curatorSerializers,
            [](ObjectT& object, const ::Arca::TypeHandle& typeHandle)
            {
                return object.Find<Arca::Curator>(typeHandle);
            });
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::FindFrom(
        TypeHandle mainTypeHandle,
        ObjectT::KnownPolymorphicSerializerList& list)
        -> ObjectT::KnownPolymorphicSerializer*
    {
        const auto found = std::find_if(
            list.begin(),
            list.end(),
            [&mainTypeHandle](const ObjectT::KnownPolymorphicSerializer& entry)
            {
                return entry.mainTypeHandle == mainTypeHandle;
            });
        return found != list.end()
            ? &*found
            : static_cast<ObjectT::KnownPolymorphicSerializer*>(nullptr);
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::PruneTypesToLoad(
        ObjectT::KnownPolymorphicSerializerList& fromObject,
        ArchiveT& archive,
        const std::vector<TypeHandle>& typeHandlesFromArchive)
        ->
        std::vector<TypeHandlePair>
    {
        auto typeHandlesFromReliquary = ExtractTypeHandles(fromObject, archive);

        std::vector<TypeHandlePair> returnValue;
        for (auto& typeHandleFromArchive : typeHandlesFromArchive)
            for (auto& typeHandleFromReliquary : typeHandlesFromReliquary)
                if (typeHandleFromReliquary.inscription == typeHandleFromArchive)
                    returnValue.push_back(typeHandleFromReliquary);

        return returnValue;
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::ExtractTypeHandles(
        ObjectT::KnownPolymorphicSerializerList& fromObject,
        ArchiveT& archive)
        ->
        std::vector<TypeHandlePair>
    {
        std::vector<TypeHandlePair> returnValue;
        for(auto& loop : fromObject)
        {
            auto inscriptionTypes = loop.inscriptionTypeProvider(archive);

            for(auto& inscriptionType : inscriptionTypes)
                returnValue.push_back({ loop.mainTypeHandle, inscriptionType });
        }
        return returnValue;
    }
}