#include "Reliquary.h"

#include <unordered_set>

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>

#include <utility>

namespace Arca
{
    Reliquary::Reliquary(Reliquary&& arg) noexcept :
        vesselMetadataList(std::move(arg.vesselMetadataList)),
        occupiedVesselIDs(std::move(arg.occupiedVesselIDs)),
        staticVesselIDMap(std::move(arg.staticVesselIDMap)),
        relicFactoryMap(std::move(arg.relicFactoryMap)),
        relicBatchSources(std::move(arg.relicBatchSources)),
        relicSerializerMap(std::move(arg.relicSerializerMap)),
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
        vesselMetadataList = std::move(arg.vesselMetadataList);
        occupiedVesselIDs = std::move(arg.occupiedVesselIDs);
        staticVesselIDMap = std::move(arg.staticVesselIDMap);
        relicFactoryMap = std::move(arg.relicFactoryMap);
        relicBatchSources = std::move(arg.relicBatchSources);
        relicSerializerMap = std::move(arg.relicSerializerMap);
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
    }

    Vessel Reliquary::CreateVessel()
    {
        const auto dynamism = VesselDynamism::Dynamic;
        const auto id = SetupNewVesselInternals(dynamism);
        return Vessel(id, dynamism, *this);
    }

    Vessel Reliquary::CreateVessel(const VesselStructure& structure)
    {
        const auto dynamism = VesselDynamism::Fixed;
        const auto id = SetupNewVesselInternals(dynamism);
        SatisfyVesselStructure(structure, id);
        return Vessel(id, dynamism, *this);
    }

    void Reliquary::ParentVessel(VesselID parent, VesselID child)
    {
        if (parent == child)
            throw CannotParentVesselToSelf(parent);

        auto parentMetadata = VesselMetadataFor(parent);
        if (!parentMetadata)
            throw CannotFindVessel(parent);

        auto childMetadata = VesselMetadataFor(child);
        if (!childMetadata)
            throw CannotFindVessel(child);

        if (childMetadata->parent.has_value())
            throw VesselAlreadyParented(child);

        parentMetadata->children.push_back(child);
        childMetadata->parent = parent;
    }

    void Reliquary::DestroyVessel(Vessel& vessel)
    {
        DestroyVessel(vessel.ID());
    }

    std::optional<Vessel> Reliquary::FindVessel(VesselID id)
    {
        const auto metadata = VesselMetadataFor(id);
        if (!metadata)
            return {};

        return Vessel(id, metadata->dynamism, *this);
    }

    Reliquary::SizeT Reliquary::VesselCount() const
    {
        return vesselMetadataList.size();
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

    VesselID Reliquary::SetupNewVesselInternals(VesselDynamism dynamism, std::optional<TypeHandle> typeHandle)
    {
        const auto id = NextVesselID();
        occupiedVesselIDs.Include(id);
        vesselMetadataList.push_back({ id, dynamism, std::move(typeHandle) });
        return id;
    }

    void Reliquary::DestroyVesselMetadata(VesselID id)
    {
        occupiedVesselIDs.Remove(id);
        const auto itr = std::remove_if(
            vesselMetadataList.begin(),
            vesselMetadataList.end(),
            [id](const VesselMetadata& metadata) { return metadata.id == id; });
        if (itr == vesselMetadataList.end())
            return;
        vesselMetadataList.erase(itr);
    }

    auto Reliquary::VesselMetadataFor(VesselID id) -> VesselMetadata*
    {
        const auto found = std::find_if(
            vesselMetadataList.begin(),
            vesselMetadataList.end(),
            [id](const VesselMetadata& metadata) { return metadata.id == id; });
        return found != vesselMetadataList.end() ? &*found : nullptr;
    }

    void Reliquary::SatisfyVesselStructure(const VesselStructure& structure, VesselID id)
    {
        for(auto& typeHandle : structure)
            CreateRelic(typeHandle, id);
    }

    void Reliquary::DestroyVessel(VesselID id)
    {
        const auto metadata = VesselMetadataFor(id);
        if (!metadata)
            return;

        if (metadata->dynamism == VesselDynamism::Static)
            return;

        for (auto& child : metadata->children)
            DestroyVessel(child);

        for (auto& relicBatchSource : relicBatchSources)
            relicBatchSource.second->DestroyFromBase(id);

        if (metadata->parent)
        {
            const auto parent = *metadata->parent;
            auto parentMetadata = VesselMetadataFor(parent);
            const auto eraseChildrenItr =
                std::remove_if(
                    parentMetadata->children.begin(),
                    parentMetadata->children.end(),
                    [id](const VesselID& childId) { return id == childId; });
            if (eraseChildrenItr != parentMetadata->children.end())
                parentMetadata->children.erase(eraseChildrenItr);
        }

        DestroyVesselMetadata(id);
    }

    VesselID Reliquary::NextVesselID() const
    {
        const auto itr = occupiedVesselIDs.begin();
        return itr == occupiedVesselIDs.end() || itr->Start() > 1
            ? 1
            : itr->Start() + 1;
    }

    void Reliquary::CreateRelic(const TypeHandle& typeHandle, VesselID id)
    {
        const auto factory = relicFactoryMap.find(typeHandle);
        if (factory == relicFactoryMap.end())
            throw NotRegistered("relic", typeHandle);

        factory->second(*this, id);
    }

    Reliquary::KnownPolymorphicSerializer::KnownPolymorphicSerializer(
        Serializer&& serializer,
        InscriptionTypeHandleProvider&& inscriptionTypeProvider)
        :
        serializer(std::move(serializer)), inscriptionTypeProvider(std::move(inscriptionTypeProvider))
    {}

    RelicBatchSourceBase* Reliquary::FindRelicBatchSource(const TypeHandle& typeHandle)
    {
        const auto found = relicBatchSources.find(typeHandle);
        if (found == relicBatchSources.end())
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
        archive(object.vesselMetadataList);

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
        archive(object.vesselMetadataList);

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