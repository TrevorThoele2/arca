#include "Reliquary.h"

#include <unordered_set>

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>
#include <Inscription/OutputJumpTable.h>
#include <Inscription/InputJumpTable.h>

#include <cassert>
#include <utility>

namespace Arca
{
    Reliquary::Reliquary(Reliquary&& arg) noexcept :
        vesselMetadataList(std::move(arg.vesselMetadataList)),
        occupiedVesselIDs(std::move(arg.occupiedVesselIDs)),
        staticVesselIDMap(std::move(arg.staticVesselIDMap)),
        relicFactoryMap(std::move(arg.relicFactoryMap)),
        relicBatchSources(std::move(arg.relicBatchSources)),
        curators(std::move(arg.curators)),
        curatorLayouts(std::move(arg.curatorLayouts)),
        signalBatchSources(std::move(arg.signalBatchSources))
    {
        for (auto& loop : curators)
            loop->Get()->owner = this;
    }

    Reliquary& Reliquary::operator=(Reliquary&& arg) noexcept
    {
        vesselMetadataList = std::move(arg.vesselMetadataList);
        occupiedVesselIDs = std::move(arg.occupiedVesselIDs);
        staticVesselIDMap = std::move(arg.staticVesselIDMap);
        relicFactoryMap = std::move(arg.relicFactoryMap);
        relicBatchSources = std::move(arg.relicBatchSources);
        curators = std::move(arg.curators);
        curatorLayouts = std::move(arg.curatorLayouts);
        signalBatchSources = std::move(arg.signalBatchSources);

        for (auto& loop : curators)
            loop->Get()->owner = this;

        return *this;
    }

    void Reliquary::Work()
    {
        for (auto& loop : curators)
            loop->Get()->Work();
    }

    Vessel Reliquary::CreateVessel()
    {
        const auto dynamism = VesselDynamism::Dynamic;
        const auto id = SetupNewVesselInternals(dynamism, false);
        return Vessel(id, dynamism, *this);
    }

    Vessel Reliquary::CreateVessel(const VesselStructure& structure)
    {
        const auto dynamism = VesselDynamism::Fixed;
        const auto id = SetupNewVesselInternals(dynamism, false);
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

    std::vector<CuratorTypeDescription> Reliquary::CuratorTypeDescriptions() const
    {
        std::vector<CuratorTypeDescription> returnValue;
        for (auto& loop : curators)
            returnValue.push_back(loop->description);
        return returnValue;
    }

    void Reliquary::Initialize()
    {
        for (auto& loop : curators)
            loop->Get()->Initialize();
    }

    Reliquary::VesselMetadata::VesselMetadata(
        VesselID id,
        VesselDynamism dynamism,
        bool isStatic,
        std::optional<TypeHandle> typeHandle)
        :
        id(id), dynamism(dynamism), isStatic(isStatic), typeHandle(std::move(typeHandle))
    {}

    VesselID Reliquary::SetupNewVesselInternals(VesselDynamism dynamism, bool isStatic, std::optional<TypeHandle> typeHandle)
    {
        const auto id = NextVesselID();
        occupiedVesselIDs.Include(id);
        vesselMetadataList.push_back({ id, dynamism, isStatic, std::move(typeHandle) });
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

        if (metadata->isStatic)
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

    RelicBatchSourceBase* Reliquary::FindRelicBatchSource(const TypeHandle& typeHandle)
    {
        const auto found = relicBatchSources.find(typeHandle);
        if (found == relicBatchSources.end())
            return nullptr;

        return found->second.get();
    }

    void Reliquary::DoOnCurators(const std::function<void(Curator*)>& function)
    {
        using RelicMap = std::unordered_set<std::type_index>;
        RelicMap used;

        const auto runFunction = [&used, &function](const std::type_index& type, Curator* curator)
        {
            function(curator);
            used.emplace(type);
        };

        for (auto& currentLayout : curatorLayouts)
        {
            for (auto& currentCuratorType : currentLayout)
            {
                if (used.find(currentCuratorType) != used.end())
                    continue;

                auto handle = std::find_if(curators.begin(), curators.end(), [&currentCuratorType](CuratorHandlePtr& handle)
                {
                    return handle->Type() == currentCuratorType;
                });
                assert(handle != curators.end());
                runFunction(currentCuratorType, (*handle)->Get());
            }
        }

        for (auto& loop : curators)
        {
            if (used.find(loop->Type()) != used.end())
                continue;

            runFunction(loop->Type(), loop->Get());
        }
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
    void Scribe<::Arca::Reliquary, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        if (archive.IsOutput())
            Save(object, archive);
        else
            Load(object, archive);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Save(ObjectT& object, ArchiveT& archive)
    {

    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Load(ObjectT& object, ArchiveT& archive)
    {

    }
}