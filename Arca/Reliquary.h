#pragma once

#include <unordered_map>
#include <optional>

#include "ReliquaryException.h"

#include "Relic.h"
#include "RelicStructure.h"
#include "RelicTraits.h"
#include "RelicMetadata.h"

#include "ShardBatch.h"
#include "ShardBatchSource.h"
#include "ShardTraits.h"

#include "SignalBatch.h"
#include "SignalBatchSource.h"
#include "ShardCreated.h"
#include "BeforeShardDestroyed.h"

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
        Relic CreateRelic();
        Relic CreateRelic(const RelicStructure& structure);
        template<class RelicT>
        RelicT CreateRelic();

        void ParentRelic(RelicID parent, RelicID child);

        void DestroyRelic(Relic& relic);
        template<class RelicT>
        void DestroyRelic(RelicT& relic);

        std::optional<Relic> FindRelic(RelicID id);
        template<class RelicT>
        std::optional<RelicT> FindRelic(RelicID id);
        template<class RelicT>
        [[nodiscard]] RelicT StaticRelic();

        [[nodiscard]] SizeT RelicCount() const;
    public:
        template<class ShardT>
        [[nodiscard]] ShardT* FindShard(RelicID id);

        template<class ShardT>
        [[nodiscard]] ShardBatch<ShardT> StartShardBatch();
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
        using RelicMetadataList = std::vector<RelicMetadata>;
        RelicMetadataList relicMetadataList;

        IntervalList<RelicID> occupiedRelicIDs;

        RelicID SetupNewRelicInternals(RelicDynamism dynamism, std::optional<TypeHandle> typeHandle = {});
        void DestroyRelicMetadata(RelicID id);
        [[nodiscard]] RelicMetadata* RelicMetadataFor(RelicID id);

        void SatisfyRelicStructure(const RelicStructure& structure, RelicID id);

        void DestroyRelic(RelicID id);

        [[nodiscard]] RelicID NextRelicID() const;
    private:
        using StaticRelicIDMap = std::unordered_map<TypeHandle, RelicID>;
        StaticRelicIDMap staticRelicIDMap;
    private:
        using ShardFactory = void(*)(Reliquary&, RelicID);
        using ShardFactoryMap = std::unordered_map<TypeHandle, ShardFactory>;
        ShardFactoryMap shardFactoryMap;

        void CreateShard(const TypeHandle& typeHandle, RelicID id);
        template<class ShardT>
        ShardT* CreateShard(RelicID id);
        template<class ShardT>
        void DestroyShard(RelicID id);

        template<class ShardT>
        void SignalCreation(ShardT& shard);
        template<class ShardT>
        void SignalDestruction(ShardT& shard);

        template<class ShardT>
        ShardFactory FindShardFactory();

        template<class ShardT>
        static TypeHandle TypeHandleForShard();
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
        using ShardBatchSourcePtr = std::unique_ptr<ShardBatchSourceBase>;
        using ShardBatchSourceMap = std::unordered_map<TypeHandle, ShardBatchSourcePtr>;
        ShardBatchSourceMap shardBatchSources;

        KnownPolymorphicSerializerMap shardSerializerMap;

        [[nodiscard]] ShardBatchSourceBase* FindShardBatchSource(const TypeHandle& typeHandle);
        template<class T>
        [[nodiscard]] ShardBatchSource<T>* FindShardBatchSource();
        template<class T>
        [[nodiscard]] ShardBatchSource<T>& RequiredShardBatchSource();
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
        friend class Relic;
        template<class... Shards>
        friend class TypedRelic;
        friend class TypeRegistration;
        template<class T>
        friend class ShardBatchSource;
        template<class T>
        friend class SignalBatchSource;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class RelicT>
    RelicT Reliquary::CreateRelic()
    {
        const auto id = SetupNewRelicInternals(RelicDynamism::Fixed, RelicTraits<RelicT>::typeHandle);
        SatisfyRelicStructure(RelicT::structure, id);
        return RelicT(id, *this);
    }

    template<class RelicT>
    void Reliquary::DestroyRelic(RelicT& relic)
    {
        DestroyRelic(relic.ID());
    }

    template<class RelicT>
    std::optional<RelicT> Reliquary::FindRelic(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            return {};

        return RelicT(id, *this);
    }

    template<class RelicT>
    RelicT Reliquary::StaticRelic()
    {
        const auto typeHandle = RelicTraits<RelicT>::typeHandle;
        const auto found = staticRelicIDMap.find(typeHandle);
        if (found == staticRelicIDMap.end())
            throw NotRegistered("static relic", typeHandle);

        return *FindRelic<RelicT>(found->second);
    }

    template<class ShardT>
    ShardT* Reliquary::FindShard(RelicID id)
    {
        auto& batch = RequiredShardBatchSource<ShardT>();
        return batch.Find(id);
    }

    template<class ShardT>
    ShardBatch<ShardT> Reliquary::StartShardBatch()
    {
        const auto typeHandle = ShardTraits<ShardT>::typeHandle;
        auto batchSource = FindShardBatchSource<ShardT>();
        if (!batchSource)
            throw NotRegistered("shard", typeHandle);

        return ShardBatch<ShardT>(*batchSource);
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

    template<class ShardT>
    ShardT* Reliquary::CreateShard(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            throw CannotCreateShard();

        if (metadata->dynamism == RelicDynamism::Fixed)
            throw CannotCreateShard();

        auto& batch = RequiredShardBatchSource<ShardT>();
        auto added = batch.Add(id);
        SignalCreation(*added);
        return added;
    }

    template<class ShardT>
    void Reliquary::DestroyShard(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            throw CannotDestroyShard();

        if (metadata->dynamism == RelicDynamism::Fixed)
            throw CannotDestroyShard();

        auto& batch = RequiredShardBatchSource<ShardT>();
        batch.Destroy(id);
    }

    template<class ShardT>
    void Reliquary::SignalCreation(ShardT& shard)
    {
        ShardCreated<ShardT> signal{ shard };
        RaiseSignal(signal);
    }

    template<class ShardT>
    void Reliquary::SignalDestruction(ShardT& shard)
    {
        BeforeShardDestroyed<ShardT> signal{ shard };
        RaiseSignal(signal);
    }

    template<class ShardT>
    auto Reliquary::FindShardFactory() -> ShardFactory
    {
        auto typeHandle = ShardTraits<ShardT>::typeHandle;
        const auto found = shardFactoryMap.find(typeHandle);
        if (found == shardFactoryMap.end())
            return nullptr;

        return found->second;
    }

    template<class ShardT>
    TypeHandle Reliquary::TypeHandleForShard()
    {
        return ShardTraits<ShardT>::typeHandle;
    }

    template<class T>
    ShardBatchSource<T>* Reliquary::FindShardBatchSource()
    {
        auto found = shardBatchSources.find(TypeHandleForShard<T>());
        if (found == shardBatchSources.end())
            return nullptr;

        return static_cast<ShardBatchSource<T>*>(found->second.get());
    }

    template<class T>
    ShardBatchSource<T>& Reliquary::RequiredShardBatchSource()
    {
        const auto typeHandle = ShardTraits<T>::typeHandle;
        auto found = FindShardBatchSource<T>();
        if (!found)
            throw NotRegistered("shard", typeHandle);

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

#include "RelicDefinition.h"