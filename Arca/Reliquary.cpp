#include "Reliquary.h"

#include <unordered_set>

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>

#include <utility>
#include "Chroma/StringUtility.h"

using namespace std::string_literals;

namespace Arca
{
    Reliquary::Reliquary(Reliquary&& arg) noexcept :
        relicMetadataList(std::move(arg.relicMetadataList)),
        occupiedRelicIDs(std::move(arg.occupiedRelicIDs)),
        namedRelicStructureList(std::move(arg.namedRelicStructureList)),
        relicBatchSources(std::move(arg.relicBatchSources)),
        relicSerializerMap(std::move(arg.relicSerializerMap)),
        staticRelicMap(std::move(arg.staticRelicMap)),
        staticRelicSerializerMap(std::move(arg.staticRelicSerializerMap)),
        shardFactoryMap(std::move(arg.shardFactoryMap)),
        shardBatchSources(std::move(arg.shardBatchSources)),
        shardSerializerMap(std::move(arg.shardSerializerMap)),
        curators(std::move(arg.curators)),
        curatorPipeline(std::move(arg.curatorPipeline)),
        curatorSerializerMap(std::move(arg.curatorSerializerMap)),
        signalBatchSources(std::move(arg.signalBatchSources))
    {
        for (auto& loop : relicBatchSources)
            loop.second->ChangeOwner(*this);

        for (auto& loop : curators)
            loop.second->Get()->owner = this;
    }

    Reliquary& Reliquary::operator=(Reliquary&& arg) noexcept
    {
        relicMetadataList = std::move(arg.relicMetadataList);
        occupiedRelicIDs = std::move(arg.occupiedRelicIDs);
        namedRelicStructureList = std::move(arg.namedRelicStructureList);
        relicBatchSources = std::move(arg.relicBatchSources);
        relicSerializerMap = std::move(arg.relicSerializerMap);
        staticRelicMap = std::move(arg.staticRelicMap);
        staticRelicSerializerMap = std::move(arg.staticRelicSerializerMap);
        shardFactoryMap = std::move(arg.shardFactoryMap);
        shardBatchSources = std::move(arg.shardBatchSources);
        shardSerializerMap = std::move(arg.shardSerializerMap);
        curators = std::move(arg.curators);
        curatorPipeline = std::move(arg.curatorPipeline);
        curatorSerializerMap = std::move(arg.curatorSerializerMap);
        signalBatchSources = std::move(arg.signalBatchSources);

        for (auto& loop : relicBatchSources)
            loop.second->ChangeOwner(*this);

        for (auto& loop : curators)
            loop.second->Get()->owner = this;

        return *this;
    }

    void Reliquary::Work()
    {
        DoOnCurators([](Curator& curator) { curator.StartStep(); });
        DoOnCurators([](Curator& curator) { curator.Work(); });
        DoOnCurators([](Curator& curator) { curator.StopStep(); });

        for(auto& signalBatchSource : signalBatchSources)
            signalBatchSource.second->Clear();
    }

    DynamicRelic Reliquary::CreateRelic()
    {
        const auto dynamism = RelicDynamism::Dynamic;
        const auto id = NextRelicID();
        SetupNewRelicInternals(id, dynamism);
        return DynamicRelic(id, dynamism, *this);
    }

    DynamicRelic Reliquary::CreateRelic(const RelicStructure& structure)
    {
        const auto dynamism = RelicDynamism::Fixed;
        const auto id = NextRelicID();
        SetupNewRelicInternals(id, dynamism);
        SatisfyRelicStructure(structure, id);
        return DynamicRelic(id, dynamism, *this);
    }

    DynamicRelic Reliquary::CreateRelic(const std::string& structureName)
    {
        for (auto& loop : namedRelicStructureList)
            if (loop.name == structureName)
                return CreateRelic(loop.value);

        throw NotRegistered("relic structure", structureName);
    }

    void Reliquary::ParentRelic(RelicID parent, RelicID child)
    {
        if (parent == child)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(parent) + ") was attempted to be parented to itself.");

        auto parentMetadata = RelicMetadataFor(parent);
        if (!parentMetadata)
            throw CannotFindRelic(parent);

        if (parentMetadata->dynamism == RelicDynamism::Static)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(child) + ") was attempted to be parented to a static relic.");

        auto childMetadata = RelicMetadataFor(child);
        if (!childMetadata)
            throw CannotFindRelic(child);

        if (childMetadata->dynamism == RelicDynamism::Static)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(child) + ") is static and cannot be parented to anything.");

        if (childMetadata->parent.has_value())
            throw CannotParentRelic(
                "The relic with id("s + Chroma::ToString(child) + ") is already parented.");;

        parentMetadata->children.push_back(child);
        childMetadata->parent = parent;

        if(childMetadata->typeHandle.has_value())
            NotifyChildRelicBatchSourcesAdd(parent, childMetadata->storage, *childMetadata->typeHandle);
    }

    void Reliquary::DestroyRelic(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!WillDestroyRelic(metadata))
            return;

        DestroyRelic(*metadata);
    }

    void Reliquary::DestroyRelic(DynamicRelic& relic)
    {
        const auto metadata = RelicMetadataFor(relic.ID());
        if (!WillDestroyRelic(metadata))
            return;

        DestroyRelic(*metadata);
    }

    std::optional<DynamicRelic> Reliquary::FindRelic(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            return {};

        return DynamicRelic(id, metadata->dynamism, *this);
    }

    Reliquary::SizeT Reliquary::RelicCount() const
    {
        return relicMetadataList.size();
    }

    Curator* Reliquary::FindCurator(const TypeHandle& typeHandle)
    {
        const auto found = curators.find(typeHandle);
        if (found == curators.end())
            return nullptr;

        return found->second->Get();
    }

    Reliquary::KnownPolymorphicSerializer::KnownPolymorphicSerializer(
        Serializer&& serializer,
        InscriptionTypeHandleProvider&& inscriptionTypeProvider)
        :
        serializer(std::move(serializer)), inscriptionTypeProvider(std::move(inscriptionTypeProvider))
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
        for(auto& typeHandle : structure)
            CreateShard(typeHandle, id);
    }

    bool Reliquary::WillDestroyRelic(RelicMetadata* metadata) const
    {
        return metadata && metadata->dynamism != RelicDynamism::Static;
    }

    void Reliquary::DestroyRelic(RelicMetadata& metadata)
    {
        auto& id = metadata.id;

        for (auto& child : metadata.children)
            DestroyRelic(child);

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

        if (metadata.parent.has_value() && metadata.typeHandle.has_value())
            NotifyChildRelicBatchSourcesDestroy(*metadata.parent, metadata.id, *metadata.typeHandle);

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

    void Reliquary::NotifyChildRelicBatchSourcesAdd(RelicID parentID, void* childStorage, const TypeHandle& childTypeHandle)
    {
        const auto found = FindChildRelicBatchSourceList(childTypeHandle);
        if (!found)
            return;

        for (auto& loop : *found)
            if (loop->Parent() == parentID)
                loop->AddFromBase(childStorage);
    }

    void Reliquary::NotifyChildRelicBatchSourcesDestroy(RelicID parentID, RelicID childID, const TypeHandle& childTypeHandle)
    {
        const auto found = FindChildRelicBatchSourceList(childTypeHandle);
        if (!found)
            return;

        for (auto& loop : *found)
            if (loop->Parent() == parentID)
                loop->DestroyFromBase(childID);
    }

    auto Reliquary::FindChildRelicBatchSource(const TypeHandle& typeHandle, RelicID parentID)
        -> ChildRelicBatchSourceBase*
    {
        auto& sourceList = RequiredChildRelicBatchSourceList(typeHandle);
        for (auto& loop : sourceList)
            if (loop->Parent() == parentID)
                return loop.get();

        return nullptr;
    }

    auto Reliquary::FindChildRelicBatchSourceList(const TypeHandle& typeHandle) -> ChildRelicBatchSourceList*
    {
        auto found = childRelicBatchSources.find(typeHandle);
        if (found == childRelicBatchSources.end())
            return nullptr;

        return &found->second;
    }

    auto Reliquary::RequiredChildRelicBatchSourceList(const TypeHandle& typeHandle) -> ChildRelicBatchSourceList&
    {
        const auto found = FindChildRelicBatchSourceList(typeHandle);
        if (found != nullptr)
            return *found;

        return childRelicBatchSources.emplace(typeHandle, ChildRelicBatchSourceList()).first->second;
    }

    void Reliquary::CreateShard(const TypeHandle& typeHandle, RelicID id)
    {
        const auto factory = shardFactoryMap.find(typeHandle);
        if (factory == shardFactoryMap.end())
            throw NotRegistered("shard", typeHandle);

        factory->second(*this, id);
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
            object.shardSerializerMap,
            object.shardBatchSources,
            [](ObjectT::ShardBatchSourceMap::value_type& entry) -> void* { return entry.second.get(); },
            [](ObjectT::ShardBatchSourceMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.relicSerializerMap,
            object.relicBatchSources,
            [](ObjectT::RelicBatchSourceMap::value_type& entry) -> void* { return entry.second.get(); },
            [](ObjectT::RelicBatchSourceMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.staticRelicSerializerMap,
            object.staticRelicMap,
            [](ObjectT::StaticRelicMap::value_type& entry) -> void* { return &entry.second; },
            [](ObjectT::StaticRelicMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.curatorSerializerMap,
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
            object.shardSerializerMap,
            [](ObjectT& object, const ::Arca::TypeHandle& typeHandle)
            {
                return object.FindShardBatchSource(typeHandle);
            });

        JumpLoadAll(
            object,
            archive,
            object.relicSerializerMap,
            [](ObjectT& object, const ::Arca::TypeHandle& typeHandle)
            {
                return object.FindRelicBatchSource(typeHandle);
            });

        JumpLoadAll(
            object,
            archive,
            object.staticRelicSerializerMap,
            [](ObjectT& object, const ::Arca::TypeHandle& typeHandle)
            {
                return &object.staticRelicMap.find(typeHandle)->second;
            });

        JumpLoadAll(
            object,
            archive,
            object.curatorSerializerMap,
            [](ObjectT& object, const ::Arca::TypeHandle& typeHandle)
            {
                return object.FindCurator(typeHandle);
            });
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::PruneTypesToLoad(
        ObjectT::KnownPolymorphicSerializerMap& fromObject,
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
        ObjectT::KnownPolymorphicSerializerMap& fromObject,
        ArchiveT& archive)
        ->
        std::vector<TypeHandlePair>
    {
        std::vector<TypeHandlePair> returnValue;
        for(auto& loop : fromObject)
        {
            auto inscriptionTypes = loop.second.inscriptionTypeProvider(archive);

            for(auto& inscriptionType : inscriptionTypes)
                returnValue.push_back({ loop.first, inscriptionType });
        }
        return returnValue;
    }
}