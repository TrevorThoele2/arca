#pragma once

#include <unordered_map>
#include <optional>

#include "ReliquaryException.h"

#include "Vessel.h"
#include "VesselStructure.h"
#include "VesselTraits.h"

#include "RelicBatch.h"
#include "RelicBatchSource.h"
#include "RelicTraits.h"

#include "SignalBatch.h"
#include "SignalBatchSource.h"
#include "RelicCreated.h"
#include "BeforeRelicDestroyed.h"

#include "Curator.h"
#include "CuratorLayout.h"
#include "CuratorHandle.h"
#include "ProcessedCuratorTraits.h"

#include "IntervalList.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary
    {
    public:
        using SizeT = size_t;
    public:
        Reliquary() = default;
        Reliquary(const Reliquary& arg) = delete;
        Reliquary(Reliquary&& arg) noexcept;
        ~Reliquary() = default;
        Reliquary& operator=(const Reliquary& arg) = delete;
        Reliquary& operator=(Reliquary&& arg) noexcept;

        void Work();
    public:
        Vessel CreateVessel();
        Vessel CreateVessel(const VesselStructure& structure);
        template<class VesselT>
        VesselT CreateVessel();

        void ParentVessel(VesselID parent, VesselID child);

        void DestroyVessel(Vessel& vessel);
        template<class VesselT>
        void DestroyVessel(VesselT& vessel);

        std::optional<Vessel> FindVessel(VesselID id);
        template<class VesselT>
        std::optional<VesselT> FindVessel(VesselID id);
        template<class VesselT>
        [[nodiscard]] VesselT StaticVessel();

        [[nodiscard]] SizeT VesselCount() const;
    public:
        template<class RelicT>
        [[nodiscard]] RelicT* FindRelic(VesselID id);

        template<class RelicT>
        [[nodiscard]] RelicBatch<RelicT> StartRelicBatch();
    public:
        template<class CuratorT>
        [[nodiscard]] CuratorT* FindCurator();
        template<class CuratorT>
        [[nodiscard]] const CuratorT* FindCurator() const;

        [[nodiscard]] std::vector<CuratorTypeDescription> CuratorTypeDescriptions() const;
    public:
        template<class SignalT>
        void RaiseSignal(const SignalT& signal);

        template<class SignalT>
        SignalBatch<SignalT> StartSignalBatch();
    private:
        void Initialize();
    private:
        struct VesselMetadata
        {
            VesselID id;
            VesselDynamism dynamism;
            std::optional<TypeHandle> typeHandle;

            std::optional<VesselID> parent;
            std::vector<VesselID> children;

            VesselMetadata(VesselID id, VesselDynamism dynamism, std::optional<TypeHandle> typeHandle = {});
        };

        using VesselMetadataList = std::vector<VesselMetadata>;
        VesselMetadataList vesselMetadataList;

        IntervalList<VesselID> occupiedVesselIDs;

        VesselID SetupNewVesselInternals(VesselDynamism dynamism);
        VesselID SetupNewVesselInternals(VesselDynamism dynamism, const TypeHandle& typeHandle);
        void DestroyVesselMetadata(VesselID id);
        [[nodiscard]] VesselMetadata* VesselMetadataFor(VesselID id);

        void SatisfyVesselStructure(const VesselStructure& structure, VesselID id);

        void DestroyVessel(VesselID id);

        [[nodiscard]] VesselID NextVesselID() const;
    private:
        using StaticVesselIDMap = std::unordered_map<TypeHandle, VesselID>;
        StaticVesselIDMap staticVesselIDMap;
    private:
        using RelicFactory = void(*)(Reliquary&, VesselID);
        using RelicFactoryMap = std::unordered_map<TypeHandle, RelicFactory>;
        RelicFactoryMap relicFactoryMap;

        void CreateRelic(const TypeHandle& typeHandle, VesselID id);
        template<class RelicT>
        RelicT* CreateRelic(VesselID id);
        template<class RelicT>
        void DestroyRelic(VesselID id);

        template<class RelicT>
        void SignalCreation(RelicT& relic);
        template<class RelicT>
        void SignalDestruction(RelicT& relic);

        template<class RelicT>
        RelicFactory FindRelicFactory();

        template<class RelicT>
        static TypeHandle TypeHandleForRelic();
    private:
        using RelicBatchSourcePtr = std::unique_ptr<RelicBatchSourceBase>;
        using RelicBatchSourceList = std::unordered_map<TypeHandle, RelicBatchSourcePtr>;
        RelicBatchSourceList relicBatchSources;

        [[nodiscard]] RelicBatchSourceBase* FindRelicBatchSource(const TypeHandle& typeHandle);
        template<class T>
        [[nodiscard]] RelicBatchSource<T>* FindRelicBatchSource();
        template<class T>
        [[nodiscard]] RelicBatchSource<T>& RequiredRelicBatchSource();
    private:
        using CuratorHandlePtr = std::unique_ptr<CuratorHandle>;
        using CuratorList = std::vector<CuratorHandlePtr>;
        CuratorList curators;

        using CuratorLayoutList = std::vector<CuratorLayout>;
        CuratorLayoutList curatorLayouts;

        template<class Curator>
        [[nodiscard]] bool HasCurator() const;

        void DoOnCurators(const std::function<void(Curator*)>& function);

        template<class Curator>
        static TypeHandle TypeHandleForCurator();
    private:
        using SignalBatchSourcePtr = std::unique_ptr<SignalBatchSourceBase>;
        using SignalBatchSourceList = std::unordered_map<std::type_index, SignalBatchSourcePtr>;
        SignalBatchSourceList signalBatchSources;

        SignalBatchSourceBase* FindSignalBatchSource(const std::type_index& type);
        template<class SignalT>
        SignalBatchSource<SignalT>* FindSignalBatchSource();
        template<class SignalT>
        const SignalBatchSource<SignalT>* FindSignalBatchSource() const;

        template<class SignalT>
        static std::type_index KeyForSignalBatchSource();
    private:
        friend class ReliquaryOrigin;
        friend class Vessel;
        template<class... Relics>
        friend class TypedVessel;
        friend class TypeRegistration;
        template<class T>
        friend class RelicBatchSource;
        template<class T>
        friend class SignalBatchSource;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class VesselT>
    VesselT Reliquary::CreateVessel()
    {
        const auto id = SetupNewVesselInternals(VesselDynamism::Fixed, VesselTraits<VesselT>::typeHandle);
        SatisfyVesselStructure(VesselT::structure);
        return VesselT(id, *this);
    }

    template<class VesselT>
    void Reliquary::DestroyVessel(VesselT& vessel)
    {
        DestroyVessel(vessel.ID());
    }

    template<class VesselT>
    std::optional<VesselT> Reliquary::FindVessel(VesselID id)
    {
        const auto metadata = VesselMetadataFor(id);
        if (!metadata)
            return {};

        return VesselT(id, *this);
    }

    template<class VesselT>
    VesselT Reliquary::StaticVessel()
    {
        auto found = staticVesselIDMap.find(VesselTraits<VesselT>::typeHandle);
        if (found == staticVesselIDMap.end())
            throw NotRegistered();

        return *FindVessel<VesselT>(found->second);
    }

    template<class RelicT>
    RelicT* Reliquary::FindRelic(VesselID id)
    {
        auto& batch = RequiredRelicBatchSource<RelicT>();
        return batch.Find(id);
    }

    template<class RelicT>
    RelicBatch<RelicT> Reliquary::StartRelicBatch()
    {
        auto batchSource = FindRelicBatchSource<RelicT>();
        if (!batchSource)
            throw NotRegistered();

        return RelicBatch<RelicT>(*batchSource);
    }

    template<class CuratorT>
    CuratorT* Reliquary::FindCurator()
    {
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(CuratorT);

        for (auto& loop : curators)
        {
            auto casted = dynamic_cast<CuratorT*>(loop->Get());
            if (casted)
                return casted;
        }

        return nullptr;
    }

    template<class CuratorT>
    const CuratorT* Reliquary::FindCurator() const
    {
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(CuratorT);

        for (auto& loop : curators)
        {
            auto casted = dynamic_cast<const CuratorT*>(loop->Get());
            if (casted)
                return casted;
        }

        return nullptr;
    }

    template<class SignalT>
    void Reliquary::RaiseSignal(const SignalT& signal)
    {
        auto signalBatchSource = FindSignalBatchSource<SignalT>();
        if (!signalBatchSource)
            throw NotRegistered();

        signalBatchSource->Raise(signal);
    }

    template<class SignalT>
    SignalBatch<SignalT> Reliquary::StartSignalBatch()
    {
        auto signalBatchSource = FindSignalBatchSource<SignalT>();
        if (!signalBatchSource)
            throw NotRegistered();

        return SignalBatch<SignalT>(*signalBatchSource);
    }

    template<class RelicT>
    RelicT* Reliquary::CreateRelic(VesselID id)
    {
        const auto metadata = VesselMetadataFor(id);
        if (!metadata)
            throw CannotCreateRelic();

        if (metadata->dynamism == VesselDynamism::Fixed)
            throw CannotCreateRelic();

        auto& batch = RequiredRelicBatchSource<RelicT>();
        auto added = batch.Add(id);
        SignalCreation(*added);
        return added;
    }

    template<class RelicT>
    void Reliquary::DestroyRelic(VesselID id)
    {
        const auto metadata = VesselMetadataFor(id);
        if (!metadata)
            throw CannotDestroyRelic();

        if (metadata->dynamism == VesselDynamism::Fixed)
            throw CannotDestroyRelic();

        auto& batch = RequiredRelicBatchSource<RelicT>();
        batch.Destroy(id);
    }

    template<class RelicT>
    void Reliquary::SignalCreation(RelicT& relic)
    {
        RelicCreated<RelicT> signal{ relic };
        RaiseSignal(signal);
    }

    template<class RelicT>
    void Reliquary::SignalDestruction(RelicT& relic)
    {
        BeforeRelicDestroyed<RelicT> signal{ relic };
        RaiseSignal(signal);
    }

    template<class RelicT>
    auto Reliquary::FindRelicFactory() -> RelicFactory
    {
        auto typeHandle = RelicTraits<RelicT>::typeHandle;
        const auto found = relicFactoryMap.find(typeHandle);
        if (found == relicFactoryMap.end())
            return nullptr;

        return found->second;
    }

    template<class RelicT>
    TypeHandle Reliquary::TypeHandleForRelic()
    {
        return RelicTraits<RelicT>::typeHandle;
    }

    template<class T>
    RelicBatchSource<T>* Reliquary::FindRelicBatchSource()
    {
        auto found = relicBatchSources.find(TypeHandleForRelic<T>());
        if (found == relicBatchSources.end())
            return nullptr;

        return static_cast<RelicBatchSource<T>*>(found->second.get());
    }

    template<class T>
    RelicBatchSource<T>& Reliquary::RequiredRelicBatchSource()
    {
        auto found = FindRelicBatchSource<T>();
        if (!found)
            throw NotRegistered();

        return *found;
    }

    template<class Curator>
    bool Reliquary::HasCurator() const
    {
        return FindCurator<Curator>() != nullptr;
    }

    template<class Curator>
    TypeHandle Reliquary::TypeHandleForCurator()
    {
        return CuratorTraits<Curator>::typeHandle;
    }

    template<class SignalT>
    SignalBatchSource<SignalT>* Reliquary::FindSignalBatchSource()
    {
        auto found = signalBatchSources.find(KeyForSignalBatchSource<SignalT>());
        if (found == signalBatchSources.end())
            return nullptr;

        return static_cast<SignalBatchSource<SignalT>*>(found->second.get());
    }

    template<class SignalT>
    const SignalBatchSource<SignalT>* Reliquary::FindSignalBatchSource() const
    {
        auto found = signalBatchSources.find(KeyForSignalBatchSource<SignalT>());
        if (found == signalBatchSources.end())
            return nullptr;

        return static_cast<SignalBatchSource<SignalT>*>(found->second.get());
    }

    template<class SignalT>
    std::type_index Reliquary::KeyForSignalBatchSource()
    {
        return typeid(SignalT);
    }
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::Reliquary, BinaryArchive> final :
        public CompositeScribe<::Arca::Reliquary, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    private:
        static void Save(ObjectT& object, ArchiveT& archive);
        static void Load(ObjectT& object, ArchiveT& archive);

        template<class Descriptions>
        static bool ShouldLoadRelic(const TypeHandle& typeHandle, const Descriptions& descriptions, ArchiveT& archive);
    };

    template<class Descriptions>
    bool Scribe<::Arca::Reliquary, BinaryArchive>::ShouldLoadRelic(const TypeHandle& typeHandle, const Descriptions& descriptions, ArchiveT& archive)
    {
        const auto anyRepresentedIsLoaded = [typeHandle, &archive](const ::Arca::RelicTypeDescription& relicDescription)
        {
            auto representedTypeHandles = relicDescription.AllSerializationRepresentedTypeHandles(archive);
            const auto anyRepresentedIsLoaded = [typeHandle](const TypeHandle& representedTypeHandle)
            {
                return representedTypeHandle == typeHandle;
            };
            return std::any_of(representedTypeHandles.begin(), representedTypeHandles.end(), anyRepresentedIsLoaded);
        };

        return std::any_of(descriptions.begin(), descriptions.end(), anyRepresentedIsLoaded);
    }
}

#include "VesselDefinition.h"