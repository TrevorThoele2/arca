#pragma once

#include <unordered_map>
#include <optional>

#include "ReliquaryException.h"

#include "Vessel.h"
#include "VesselStructure.h"
#include "VesselTraits.h"
#include "VesselMetadata.h"

#include "RelicBatch.h"
#include "RelicBatchSource.h"
#include "RelicTraits.h"

#include "SignalBatch.h"
#include "SignalBatchSource.h"
#include "RelicCreated.h"
#include "BeforeRelicDestroyed.h"

#include "Curator.h"
#include "CuratorPipeline.h"
#include "CuratorHandle.h"
#include "CuratorTraits.h"

#include "IntervalList.h"

#include "Serialization.h"
#include <Inscription/OutputJumpTable.h>
#include <Inscription/InputJumpTable.h>

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
    public:
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
        Curator* FindCurator(const TypeHandle& typeHandle);
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
        using VesselMetadataList = std::vector<VesselMetadata>;
        VesselMetadataList vesselMetadataList;

        IntervalList<VesselID> occupiedVesselIDs;

        VesselID SetupNewVesselInternals(VesselDynamism dynamism, std::optional<TypeHandle> typeHandle = {});
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
        struct KnownPolymorphicSerializer
        {
            using Serializer = std::function<void(void*, ::Inscription::BinaryArchive&)>;
            const Serializer serializer;

            using InscriptionTypeHandles = std::vector<::Inscription::TypeHandle>;
            using InscriptionTypeHandleProvider =
                std::function<InscriptionTypeHandles(::Inscription::BinaryArchive&)>;
            InscriptionTypeHandleProvider inscriptionTypeProvider;

            KnownPolymorphicSerializer(
                Serializer&& serializer,
                InscriptionTypeHandleProvider&& inscriptionTypeProvider);
        };
        using KnownPolymorphicSerializerMap = std::unordered_map<TypeHandle, KnownPolymorphicSerializer>;
    private:
        using RelicBatchSourcePtr = std::unique_ptr<RelicBatchSourceBase>;
        using RelicBatchSourceMap = std::unordered_map<TypeHandle, RelicBatchSourcePtr>;
        RelicBatchSourceMap relicBatchSources;

        KnownPolymorphicSerializerMap relicSerializerMap;

        [[nodiscard]] RelicBatchSourceBase* FindRelicBatchSource(const TypeHandle& typeHandle);
        template<class T>
        [[nodiscard]] RelicBatchSource<T>* FindRelicBatchSource();
        template<class T>
        [[nodiscard]] RelicBatchSource<T>& RequiredRelicBatchSource();
    private:
        using CuratorHandlePtr = std::unique_ptr<CuratorHandle>;
        using CuratorMap = std::unordered_map<TypeHandle, CuratorHandlePtr>;
        CuratorMap curators;

        using CuratorStage = std::vector<Curator*>;
        using CuratorPipeline = std::vector<CuratorStage>;
        CuratorPipeline curatorPipeline;

        KnownPolymorphicSerializerMap curatorSerializerMap;

        template<class Curator>
        [[nodiscard]] bool HasCurator() const;

        template<class Function>
        void DoOnCurators(Function function);

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
        SatisfyVesselStructure(VesselT::structure, id);
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
        const auto typeHandle = VesselTraits<VesselT>::typeHandle;
        const auto found = staticVesselIDMap.find(typeHandle);
        if (found == staticVesselIDMap.end())
            throw NotRegistered("static vessel", typeHandle);

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
        const auto typeHandle = RelicTraits<RelicT>::typeHandle;
        auto batchSource = FindRelicBatchSource<RelicT>();
        if (!batchSource)
            throw NotRegistered("relic", typeHandle);

        return RelicBatch<RelicT>(*batchSource);
    }

    template<class CuratorT>
    CuratorT* Reliquary::FindCurator()
    {
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(CuratorT);

        for (auto& loop : curators)
        {
            auto casted = dynamic_cast<CuratorT*>(loop.second->Get());
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
            auto casted = dynamic_cast<const CuratorT*>(loop.second->Get());
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
            throw NotRegistered("signal", typeid(SignalT).name());

        signalBatchSource->Raise(signal);
    }

    template<class SignalT>
    SignalBatch<SignalT> Reliquary::StartSignalBatch()
    {
        auto signalBatchSource = FindSignalBatchSource<SignalT>();
        if (!signalBatchSource)
            throw NotRegistered("signal", typeid(SignalT).name());

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
        const auto typeHandle = RelicTraits<T>::typeHandle;
        auto found = FindRelicBatchSource<T>();
        if (!found)
            throw NotRegistered("relic", typeHandle);

        return *found;
    }

    template<class Curator>
    bool Reliquary::HasCurator() const
    {
        return FindCurator<Curator>() != nullptr;
    }

    template<class Function>
    void Reliquary::DoOnCurators(Function function)
    {
        if (curatorPipeline.empty())
        {
            for (auto& curator : curators)
                function(*curator.second->Get());
        }
        else
        {
            for (auto& stage : curatorPipeline)
                for (auto& curator : stage)
                    function(*curator);
        }
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
    class KnownPolymorphic
    {
    public:
        using Serializer = std::function<void(void*, BinaryArchive&)>;
    public:
        explicit KnownPolymorphic(void* underlying, Serializer serializer);
    private:
        void* underlying;
        Serializer serializer;
    private:
        INSCRIPTION_ACCESS;
    };

    template<>
    class Scribe<KnownPolymorphic, BinaryArchive> final :
        public CompositeScribe<KnownPolymorphic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<>
    class Scribe<::Arca::Reliquary, BinaryArchive> final :
        public CompositeScribe<::Arca::Reliquary, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    private:
        static void Save(ObjectT& object, ArchiveT& archive);
        static void Load(ObjectT& object, ArchiveT& archive);

        template<class ObjectContainer, class ValueToPiece, class ValueToID>
        static void JumpSaveAll(
            ObjectT& object,
            ArchiveT& archive,
            ObjectT::KnownPolymorphicSerializerMap& fromObject,
            ObjectContainer& container,
            ValueToPiece valueToPiece,
            ValueToID valueToID);

        template<class FindObject>
        static void JumpLoadAll(
            ObjectT& object,
            ArchiveT& archive,
            ObjectT::KnownPolymorphicSerializerMap& fromObject,
            FindObject findSerializedObject);
    private:
        struct TypeHandlePair
        {
            ::Arca::TypeHandle arca;
            TypeHandle inscription;
        };

        static std::vector<TypeHandlePair> PruneTypesToLoad(
            ObjectT::KnownPolymorphicSerializerMap& fromObject,
            ArchiveT& archive,
            const std::vector<TypeHandle>& typeHandlesFromArchive);

        static std::vector<TypeHandlePair> ExtractTypeHandles(
            ObjectT::KnownPolymorphicSerializerMap& fromObject,
            ArchiveT& archive);
    };

    template<class ObjectContainer, class ValueToPiece, class ValueToID>
    void Scribe<::Arca::Reliquary, BinaryArchive>::JumpSaveAll(
        ObjectT& object,
        ArchiveT& archive,
        ObjectT::KnownPolymorphicSerializerMap& fromObject,
        ObjectContainer& container,
        ValueToPiece valueToPiece,
        ValueToID valueToID)
    {
        OutputJumpTable<TypeHandle, KnownPolymorphic> jumpTable;
        std::vector<KnownPolymorphic> knownPolymorphics;
        for (auto& loop : container)
        {
            auto id = valueToID(loop);
            void* piece = valueToPiece(loop);
            const KnownPolymorphic::Serializer* serializer = &fromObject.find(id)->second.serializer;
            knownPolymorphics.emplace_back(piece, *serializer);
            jumpTable.Add(id, knownPolymorphics.back());
        }

        archive(jumpTable);
    }

    template<class FindObject>
    void Scribe<::Arca::Reliquary, BinaryArchive>::JumpLoadAll(
        ObjectT& object,
        ArchiveT& archive,
        ObjectT::KnownPolymorphicSerializerMap& fromObject,
        FindObject findSerializedObject)
    {
        InputJumpTable<TypeHandle, KnownPolymorphic> jumpTable;
        archive(jumpTable);

        auto typesToLoad = PruneTypesToLoad(
            fromObject,
            archive,
            jumpTable.AllIDs());

        for (auto& typeToLoad : typesToLoad)
        {
            const auto serializedObject = findSerializedObject(object, typeToLoad.arca);
            const auto serializer = &fromObject.find(typeToLoad.arca)->second.serializer;
            auto adapter = KnownPolymorphic(serializedObject, *serializer);
            jumpTable.FillObject(typeToLoad.inscription, adapter, archive);
        }
    }
}

#include "VesselDefinition.h"