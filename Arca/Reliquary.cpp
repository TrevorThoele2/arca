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
        staticRelicIDMap(std::move(arg.staticRelicIDMap)),
        shardFactoryMap(std::move(arg.shardFactoryMap)),
        shardBatchSources(std::move(arg.shardBatchSources)),
        shardSerializerMap(std::move(arg.shardSerializerMap)),
        curators(std::move(arg.curators)),
        curatorPipeline(std::move(arg.curatorPipeline)),
        curatorSerializerMap(std::move(arg.curatorSerializerMap)),
        signalBatchSources(std::move(arg.signalBatchSources))
    {
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
        staticRelicIDMap = std::move(arg.staticRelicIDMap);
        shardFactoryMap = std::move(arg.shardFactoryMap);
        shardBatchSources = std::move(arg.shardBatchSources);
        shardSerializerMap = std::move(arg.shardSerializerMap);
        curators = std::move(arg.curators);
        curatorPipeline = std::move(arg.curatorPipeline);
        curatorSerializerMap = std::move(arg.curatorSerializerMap);
        signalBatchSources = std::move(arg.signalBatchSources);

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

    Relic Reliquary::CreateRelic()
    {
        const auto dynamism = RelicDynamism::Dynamic;
        const auto id = SetupNewRelicInternals(dynamism);
        return Relic(id, dynamism, *this);
    }

    Relic Reliquary::CreateRelic(const RelicStructure& structure)
    {
        const auto dynamism = RelicDynamism::Fixed;
        const auto id = SetupNewRelicInternals(dynamism);
        SatisfyRelicStructure(structure, id);
        return Relic(id, dynamism, *this);
    }

    Relic Reliquary::CreateRelic(const std::string& structureName)
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
    }

    void Reliquary::DestroyRelic(Relic& relic)
    {
        DestroyRelic(relic.ID());
    }

    std::optional<Relic> Reliquary::FindRelic(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            return {};

        return Relic(id, metadata->dynamism, *this);
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

    std::vector<CuratorTypeDescription> Reliquary::CuratorTypeDescriptions() const
    {
        std::vector<CuratorTypeDescription> returnValue;
        for (auto& loop : curators)
            returnValue.push_back(loop.second->description);
        return returnValue;
    }

    void Reliquary::Initialize()
    {
        for (auto& loop : curators)
            loop.second->Get()->Initialize();
    }

    Reliquary::KnownPolymorphicSerializer::KnownPolymorphicSerializer(
        Serializer&& serializer,
        InscriptionTypeHandleProvider&& inscriptionTypeProvider)
        :
        serializer(std::move(serializer)), inscriptionTypeProvider(std::move(inscriptionTypeProvider))
    {}

    RelicID Reliquary::SetupNewRelicInternals(RelicDynamism dynamism, std::optional<TypeHandle> typeHandle)
    {
        const auto id = NextRelicID();
        occupiedRelicIDs.Include(id);
        relicMetadataList.push_back({ id, dynamism, std::move(typeHandle) });
        return id;
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

    void Reliquary::SatisfyRelicStructure(const RelicStructure& structure, RelicID id)
    {
        for(auto& typeHandle : structure)
            CreateShard(typeHandle, id);
    }

    void Reliquary::DestroyRelic(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            return;

        if (metadata->dynamism == RelicDynamism::Static)
            return;

        for (auto& child : metadata->children)
            DestroyRelic(child);

        for (auto& shardBatchSource : shardBatchSources)
            shardBatchSource.second->DestroyFromBase(id);

        if (metadata->parent)
        {
            const auto parent = *metadata->parent;
            auto parentMetadata = RelicMetadataFor(parent);
            const auto eraseChildrenItr =
                std::remove_if(
                    parentMetadata->children.begin(),
                    parentMetadata->children.end(),
                    [id](const RelicID& childId) { return id == childId; });
            if (eraseChildrenItr != parentMetadata->children.end())
                parentMetadata->children.erase(eraseChildrenItr);
        }

        if (metadata->typeHandle)
        {
            auto batchSource = FindRelicBatchSource(*metadata->typeHandle);
            batchSource->DestroyFromBase(id);
        }

        DestroyRelicMetadata(id);
    }

    RelicID Reliquary::NextRelicID() const
    {
        const auto itr = occupiedRelicIDs.begin();
        return itr == occupiedRelicIDs.end() || itr->Start() > 1
            ? 1
            : itr->Start() + 1;
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
    KnownPolymorphic::KnownPolymorphic(void* underlying, Serializer serializer) :
        underlying(underlying), serializer(std::move(serializer))
    {}

    void Scribe<KnownPolymorphic, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        object.serializer(object.underlying, archive);
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
            [](const ObjectT::ShardBatchSourceMap::value_type& entry) { return entry.second.get(); },
            [](const ObjectT::ShardBatchSourceMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.relicSerializerMap,
            object.relicBatchSources,
            [](const ObjectT::RelicBatchSourceMap::value_type& entry) { return entry.second.get(); },
            [](const ObjectT::RelicBatchSourceMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.curatorSerializerMap,
            object.curators,
            [](const ObjectT::CuratorMap::value_type& entry) { return entry.second->Get(); },
            [](const ObjectT::CuratorMap::value_type& entry) { return entry.first; });
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