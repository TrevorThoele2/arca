#pragma once

#include <unordered_map>
#include <optional>
#include <any>

#include "ReliquaryException.h"

#include "DynamicRelic.h"
#include "FixedRelic.h"
#include "TypedRelic.h"
#include "RelicStructure.h"
#include "RelicTraits.h"
#include "HasFactoryMethod.h"
#include "StructureFrom.h"
#include "RelicMetadata.h"

#include "RelicBatch.h"
#include "RelicBatchSource.h"

#include "ShardBatch.h"
#include "ShardBatchSource.h"
#include "ShardTraits.h"

#include "SignalBatch.h"
#include "SignalBatchSource.h"
#include "SignalTraits.h"
#include "Destroying.h"
#include "Created.h"

#include "Curator.h"
#include "CuratorHandle.h"
#include "CuratorTraits.h"

#include "Ptr.h"
#include "Static.h"

#include "IntervalList.h"

#include "Chroma/StringUtility.h"
using namespace std::string_literals;

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
        Reliquary(Reliquary&& arg) noexcept = delete;
        ~Reliquary() = default;
        Reliquary& operator=(const Reliquary& arg) = delete;
        Reliquary& operator=(Reliquary&& arg) noexcept = delete;
    public:
        void Work();
    public:
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, DynamicRelic>, int> = 0>
        DynamicRelic Create();
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, FixedRelic>, int> = 0>
        FixedRelic Create(const RelicStructure& structure);
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, FixedRelic>, int> = 0>
        FixedRelic Create(const std::string& structureName);
        template<
            class RelicT,
            class... CreationArgs,
            std::enable_if_t<is_relic_v<RelicT> && has_factory_method_v<RelicT>, int> = 0>
        Ptr<RelicT> Create(CreationArgs&& ... creationArgs);
        template<
            class RelicT,
            class... CreationArgs,
            std::enable_if_t<is_relic_v<RelicT> && !has_factory_method_v<RelicT>, int> = 0>
        Ptr<RelicT> Create(CreationArgs&& ... creationArgs);

        void Destroy(const RelicHandle& handle);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Destroy(RelicT& relic);

        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, DynamicRelic>, int> = 0>
        std::optional<DynamicRelic> Find(RelicID id);
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, FixedRelic>, int> = 0>
        std::optional<FixedRelic> Find(RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> Find(RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Arca::Static<RelicT> Static();

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Arca::Batch<RelicT> Batch();

        void ParentRelic(const RelicHandle& parent, const RelicHandle& child);

        [[nodiscard]] SizeT RelicSize() const;
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Ptr<ShardT> Find(RelicID id);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Arca::Batch<ShardT> Batch();
    public:
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        Curator* Find(const TypeHandle& typeHandle);
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] CuratorT* Find();
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] const CuratorT* Find() const;
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(const SignalT& signal);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void ExecuteOn(const std::function<void(const SignalT&)>& function);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        Arca::Batch<SignalT> Batch();
    private:
        struct KnownPolymorphicSerializer
        {
            const TypeHandle mainTypeHandle;

            using Serializer = std::function<void(void*, Reliquary&, ::Inscription::BinaryArchive&)>;
            const Serializer serializer;

            using InscriptionTypeHandles = std::vector<::Inscription::TypeHandle>;
            using InscriptionTypeHandleProvider =
                std::function<InscriptionTypeHandles(::Inscription::BinaryArchive&)>;
            InscriptionTypeHandleProvider inscriptionTypeProvider;

            KnownPolymorphicSerializer(
                TypeHandle mainTypeHandle,
                Serializer&& serializer,
                InscriptionTypeHandleProvider&& inscriptionTypeProvider);
        };
        using KnownPolymorphicSerializerList = std::list<KnownPolymorphicSerializer>;
    private:
        using RelicMetadataList = std::vector<RelicMetadata>;
        RelicMetadataList relicMetadataList;

        IntervalList<RelicID> occupiedRelicIDs;

        void SetupNewRelicInternals(
            RelicID id,
            RelicDynamism dynamism,
            std::optional<TypeHandle> typeHandle = {},
            void* storage = nullptr);
        void DestroyRelicMetadata(RelicID id);
        [[nodiscard]] RelicMetadata* RelicMetadataFor(RelicID id);
        [[nodiscard]] const RelicMetadata* RelicMetadataFor(RelicID id) const;

        void SatisfyRelicStructure(const RelicStructure& structure, RelicID id);

        [[nodiscard]] bool WillDestroyRelic(RelicMetadata* metadata) const;
        void DestroyRelic(RelicMetadata& metadata);

        [[nodiscard]] RelicID NextRelicID() const;

        void SignalRelicParented(const RelicHandle& parent, const RelicHandle& child);

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindStorage(RelicID id);
    private:
        struct NamedRelicStructure
        {
            std::string name;
            RelicStructure value;

            NamedRelicStructure(std::string name, Arca::RelicStructure value);
        };
        using NamedRelicStructureList = std::vector<NamedRelicStructure>;
        NamedRelicStructureList namedRelicStructureList;
    private:
        using RelicBatchSourcePtr = std::unique_ptr<RelicBatchSourceBase>;
        using RelicBatchSourceMap = std::unordered_map<TypeHandle, RelicBatchSourcePtr>;
        RelicBatchSourceMap relicBatchSources;

        KnownPolymorphicSerializerList relicSerializers;

        [[nodiscard]] RelicBatchSourceBase* FindRelicBatchSource(const TypeHandle& typeHandle);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] BatchSource<RelicT>* FindBatchSource();
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] BatchSource<RelicT>& RequiredBatchSource();
    private:
        using StaticRelicMap = std::unordered_map<TypeHandle, std::any>;
        StaticRelicMap staticRelicMap;

        KnownPolymorphicSerializerList staticRelicSerializers;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindStaticStorage(RelicID id);
    private:
        using ShardFactory = void(*)(Reliquary&, RelicID, bool);
        using ShardFactoryMap = std::unordered_map<TypeHandle, ShardFactory>;
        ShardFactoryMap shardFactoryMap;

        void CreateShard(const TypeHandle& typeHandle, RelicID id, bool isConst);
        template<class ShardT>
        Ptr<ShardT> CreateShard(RelicID id);
        template<class ShardT>
        void DestroyShard(RelicID id);

        template<class ShardT>
        void SignalShardCreation(ShardT& shard);
        template<class ShardT>
        void SignalShardDestruction(ShardT& shard);

        template<class ShardT>
        ShardFactory FindShardFactory();

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
    private:
        using ShardBatchSourcePtr = std::unique_ptr<ShardBatchSourceBase>;
        using ShardBatchSourceMap = std::unordered_map<TypeHandle, ShardBatchSourcePtr>;
        ShardBatchSourceMap shardBatchSources;

        KnownPolymorphicSerializerList shardSerializers;

        [[nodiscard]] ShardBatchSourceBase* FindShardBatchSource(const TypeHandle& typeHandle);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] BatchSource<ShardT>* FindBatchSource();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] BatchSource<ShardT>& RequiredBatchSource();
    private:
        using CuratorHandlePtr = std::unique_ptr<CuratorHandle>;
        using CuratorMap = std::unordered_map<TypeHandle, CuratorHandlePtr>;
        CuratorMap curators;

        using CuratorStage = std::vector<Curator*>;
        using CuratorPipeline = std::vector<CuratorStage>;
        CuratorPipeline curatorWorkPipeline;

        KnownPolymorphicSerializerList curatorSerializers;

        template<class CuratorT>
        [[nodiscard]] bool HasCurator() const;

        template<class Function>
        void WorkCurators(Function function);
    private:
        template<class SignalT>
        using SignalExecution = std::function<void(const SignalT&)>;
        template<class SignalT>
        using SignalExecutionList = std::vector<SignalExecution<SignalT>>;
        using SignalExecutionMap = std::unordered_map<std::type_index, std::any>;
        SignalExecutionMap signalExecutionMap;

        template<class SignalT>
        void ExecuteAllForSignal(const SignalT& signal);
    private:
        using SignalBatchSourcePtr = std::unique_ptr<SignalBatchSourceBase>;
        using SignalBatchSourceList = std::unordered_map<std::type_index, SignalBatchSourcePtr>;
        SignalBatchSourceList signalBatchSources;

        SignalBatchSourceBase* FindSignalBatchSource(const std::type_index& type);
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        BatchSource<SignalT>* FindBatchSource();
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        const BatchSource<SignalT>* FindBatchSource() const;

        template<class SignalT>
        static std::type_index KeyForSignalBatchSource();
    private:
        template<class T>
        void SignalCreation(T& object);
        template<class T>
        void SignalDestroying(T& object);
    private:
        friend class ReliquaryOrigin;
        friend class DynamicRelic;
        friend class TypedRelic;
        template<class T>
        friend class Ptr;
        template<class T>
        friend class Static;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, DynamicRelic>, int>>
    DynamicRelic Reliquary::Create()
    {
        const auto dynamism = RelicDynamism::Dynamic;
        const auto id = NextRelicID();
        SetupNewRelicInternals(id, dynamism);
        return DynamicRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, FixedRelic>, int>>
    FixedRelic Reliquary::Create(const RelicStructure& structure)
    {
        const auto dynamism = RelicDynamism::Fixed;
        const auto id = NextRelicID();
        SetupNewRelicInternals(id, dynamism);
        SatisfyRelicStructure(structure, id);
        return FixedRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, FixedRelic>, int>>
    FixedRelic Reliquary::Create(const std::string& structureName)
    {
        for (auto& loop : namedRelicStructureList)
            if (loop.name == structureName)
                return Create<FixedRelic>(loop.value);

        throw NotRegistered("relic structure", structureName);
    }

    template<
        class RelicT,
        class... CreationArgs,
        std::enable_if_t<is_relic_v<RelicT> && has_factory_method_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::Create(CreationArgs&& ... creationArgs)
    {
        const auto id = NextRelicID();
        std::optional<RelicT> relic = Traits<RelicT>::Factory(*this, std::forward<CreationArgs>(creationArgs)...);
        if (!relic)
            return {};
        relic->id = id;

        auto& batchSource = RequiredBatchSource<RelicT>();
        auto added = batchSource.Add(*relic);

        SetupNewRelicInternals(id, RelicDynamism::Fixed, TypeHandleFor<RelicT>(), added);
        SatisfyRelicStructure(StructureFrom<shards_for_t<RelicT>>(), id);
        SignalCreation(*added);

        added->Initialize(*this);

        return Ptr<RelicT>(id, *this);
    }

    template<
        class RelicT,
        class... CreationArgs,
        std::enable_if_t<is_relic_v<RelicT> && !has_factory_method_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::Create(CreationArgs&& ... creationArgs)
    {
        const auto id = NextRelicID();
        RelicT relic(std::forward<CreationArgs>(creationArgs)...);
        relic.id = id;

        auto& batchSource = RequiredBatchSource<RelicT>();
        auto added = batchSource.Add(relic);

        SetupNewRelicInternals(id, RelicDynamism::Fixed, TypeHandleFor<RelicT>(), added);
        SatisfyRelicStructure(StructureFrom<shards_for_t<RelicT>>(), id);
        SignalCreation(*added);

        added->Initialize(*this);

        return Ptr<RelicT>(id, *this);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void Reliquary::Destroy(RelicT& relic)
    {
        const auto metadata = RelicMetadataFor(relic.ID());
        if (!WillDestroyRelic(metadata))
            return;

        SignalDestroying(relic);
        DestroyRelic(*metadata);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, DynamicRelic>, int>>
    std::optional<DynamicRelic> Reliquary::Find(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata || metadata->dynamism != RelicDynamism::Dynamic)
            return {};

        return DynamicRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, FixedRelic>, int>>
    std::optional<FixedRelic> Reliquary::Find(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata || metadata->dynamism != RelicDynamism::Fixed)
            return {};

        return FixedRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::Find(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            return {};

        auto& batchSource = RequiredBatchSource<RelicT>();
        auto found = batchSource.Find(id);
        if (found == nullptr)
            return {};

        return Ptr<RelicT>(id, *this);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    Static<RelicT> Reliquary::Static()
    {
        const auto typeHandle = TypeHandleFor<RelicT>();
        const auto found = staticRelicMap.find(typeHandle);
        if (found == staticRelicMap.end())
            throw NotRegistered("static relic", typeHandle, typeid(RelicT));

        auto staticRelic = std::any_cast<RelicT>(&found->second);
        return Arca::Static<RelicT>(staticRelic->ID(), *this);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    Arca::Batch<RelicT> Reliquary::Batch()
    {
        const auto typeHandle = TypeHandleFor<RelicT>();
        auto& batchSource = RequiredBatchSource<RelicT>();
        return Arca::Batch<RelicT>(batchSource);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> Reliquary::Find(RelicID id)
    {
        auto& batch = RequiredBatchSource<std::decay_t<ShardT>>();
        auto found = batch.Find(id, std::is_const_v<ShardT>);
        if (!found)
            return {};

        return Ptr<ShardT>(id, *this);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Arca::Batch<ShardT> Reliquary::Batch()
    {
        const auto typeHandle = TypeHandleFor<ShardT>();
        auto batchSource = FindBatchSource<std::decay_t<ShardT>>();
        if (!batchSource)
            throw NotRegistered("shard", typeHandle, typeid(ShardT));

        return Arca::Batch<ShardT>(*batchSource);
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    Curator* Reliquary::Find(const TypeHandle& typeHandle)
    {
        const auto found = curators.find(typeHandle);
        if (found == curators.end())
            return nullptr;

        return found->second->Get();
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    CuratorT* Reliquary::Find()
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

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    const CuratorT* Reliquary::Find() const
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

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void Reliquary::Raise(const SignalT& signal)
    {
        auto signalBatchSource = FindBatchSource<SignalT>();
        if (!signalBatchSource)
            throw NotRegistered("signal", typeid(SignalT).name());

        signalBatchSource->Raise(signal);
        ExecuteAllForSignal(signal);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void Reliquary::ExecuteOn(const std::function<void(const SignalT&)>& function)
    {
        const auto typeIndex = std::type_index(typeid(SignalT));
        auto found = signalExecutionMap.find(typeIndex);
        if (found == signalExecutionMap.end())
            found = signalExecutionMap.emplace(typeIndex, SignalExecutionList<SignalT>()).first;

        auto& executionList = std::any_cast<SignalExecutionList<SignalT>&>(found->second);
        executionList.push_back(function);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    Arca::Batch<SignalT> Reliquary::Batch()
    {
        auto signalBatchSource = FindBatchSource<SignalT>();
        if (!signalBatchSource)
            throw NotRegistered("signal", typeid(SignalT).name());

        return Arca::Batch<SignalT>(*signalBatchSource);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* Reliquary::FindStorage(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            return {};

        auto& batchSource = RequiredBatchSource<RelicT>();
        return batchSource.Find(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    BatchSource<RelicT>* Reliquary::FindBatchSource()
    {
        auto found = relicBatchSources.find(TypeHandleFor<RelicT>());
        if (found == relicBatchSources.end())
            return nullptr;

        return static_cast<BatchSource<RelicT>*>(found->second.get());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    BatchSource<RelicT>& Reliquary::RequiredBatchSource()
    {
        const auto typeHandle = TypeHandleFor<RelicT>();
        auto found = FindBatchSource<RelicT>();
        if (!found)
            throw NotRegistered("relic", typeHandle, typeid(RelicT));

        return *found;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* Reliquary::FindStaticStorage(RelicID id)
    {
        const auto typeHandle = TypeHandleFor<RelicT>();
        const auto found = staticRelicMap.find(typeHandle);
        if (found == staticRelicMap.end())
            throw NotRegistered("static relic", typeHandle, typeid(RelicT));

        return std::any_cast<RelicT>(&found->second);
    }

    template<class ShardT>
    Ptr<ShardT> Reliquary::CreateShard(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            throw CannotCreateShard();

        if (metadata->dynamism == RelicDynamism::Fixed)
            throw CannotCreateShard();

        auto& batch = RequiredBatchSource<std::decay_t<ShardT>>();
        auto added = batch.Add(id, std::is_const_v<ShardT>);
        SignalShardCreation(*added);
        return Ptr<ShardT>(id, *this);
    }

    template<class ShardT>
    void Reliquary::DestroyShard(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            throw CannotDestroyShard();

        if (metadata->dynamism == RelicDynamism::Fixed)
            throw CannotDestroyShard();

        auto& batch = RequiredBatchSource<ShardT>();
        batch.Destroy(id);
    }

    template<class ShardT>
    void Reliquary::SignalShardCreation(ShardT& shard)
    {
        Created<ShardT> signal{ shard };
        Raise(signal);
    }

    template<class ShardT>
    void Reliquary::SignalShardDestruction(ShardT& shard)
    {
        Destroying<ShardT> signal{ shard };
        Raise(signal);
    }

    template<class ShardT>
    auto Reliquary::FindShardFactory() -> ShardFactory
    {
        auto typeHandle = TypeHandleFor<ShardT>();
        const auto found = shardFactoryMap.find(typeHandle);
        if (found == shardFactoryMap.end())
            return nullptr;

        return found->second;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Reliquary::FindStorage(RelicID id)
    {
        auto& batch = RequiredBatchSource<std::decay_t<ShardT>>();
        return batch.Find(id, std::is_const_v<ShardT>);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    BatchSource<ShardT>* Reliquary::FindBatchSource()
    {
        auto found = shardBatchSources.find(TypeHandleFor<ShardT>());
        if (found == shardBatchSources.end())
            return nullptr;

        return static_cast<BatchSource<ShardT>*>(found->second.get());
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    BatchSource<ShardT>& Reliquary::RequiredBatchSource()
    {
        const auto typeHandle = TypeHandleFor<ShardT>();
        auto found = FindBatchSource<ShardT>();
        if (!found)
            throw NotRegistered("shard", typeHandle, typeid(ShardT));

        return *found;
    }

    template<class CuratorT>
    bool Reliquary::HasCurator() const
    {
        return Find<CuratorT>() != nullptr;
    }

    template<class Function>
    void Reliquary::WorkCurators(Function function)
    {
        for (auto& stage : curatorWorkPipeline)
            for (auto& curator : stage)
                function(*curator);
    }

    template<class SignalT>
    void Reliquary::ExecuteAllForSignal(const SignalT& signal)
    {
        const auto typeIndex = std::type_index(typeid(SignalT));
        auto found = signalExecutionMap.find(typeIndex);
        if (found == signalExecutionMap.end())
            return;

        auto& executionList = std::any_cast<SignalExecutionList<SignalT>&>(found->second);
        for (auto& loop : executionList)
            loop(signal);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    BatchSource<SignalT>* Reliquary::FindBatchSource()
    {
        auto found = signalBatchSources.find(KeyForSignalBatchSource<SignalT>());
        if (found == signalBatchSources.end())
            return nullptr;

        return static_cast<BatchSource<SignalT>*>(found->second.get());
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    const BatchSource<SignalT>* Reliquary::FindBatchSource() const
    {
        auto found = signalBatchSources.find(KeyForSignalBatchSource<SignalT>());
        if (found == signalBatchSources.end())
            return nullptr;

        return static_cast<BatchSource<SignalT>*>(found->second.get());
    }

    template<class SignalT>
    std::type_index Reliquary::KeyForSignalBatchSource()
    {
        return typeid(SignalT);
    }

    template<class T>
    void Reliquary::SignalCreation(T& object)
    {
        Created<T> signal{ object };
        Raise(signal);
    }

    template<class T>
    void Reliquary::SignalDestroying(T& object)
    {
        Destroying<T> signal{ object };
        Raise(signal);
    }
}

namespace Inscription
{
    class KnownPolymorphic
    {
    public:
        using Serializer = std::function<void(void*, Arca::Reliquary&, BinaryArchive&)>;
    public:
        explicit KnownPolymorphic(void* underlying, Arca::Reliquary& reliquary, Serializer serializer);
    private:
        void* underlying;
        Arca::Reliquary* reliquary;
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
            ObjectT::KnownPolymorphicSerializerList& fromObject,
            ObjectContainer& container,
            ValueToPiece valueToPiece,
            ValueToID valueToID);

        template<class FindObject>
        static void JumpLoadAll(
            ObjectT& object,
            ArchiveT& archive,
            ObjectT::KnownPolymorphicSerializerList& fromObject,
            FindObject findSerializedObject);

        static ObjectT::KnownPolymorphicSerializer* FindFrom(
            TypeHandle mainTypeHandle,
            ObjectT::KnownPolymorphicSerializerList& list);
    private:
        struct TypeHandlePair
        {
            ::Arca::TypeHandle arca;
            TypeHandle inscription;
        };

        static std::vector<TypeHandlePair> PruneTypesToLoad(
            ObjectT::KnownPolymorphicSerializerList& fromObject,
            ArchiveT& archive,
            const std::vector<TypeHandle>& typeHandlesFromArchive);

        static std::vector<TypeHandlePair> ExtractTypeHandles(
            ObjectT::KnownPolymorphicSerializerList& fromObject,
            ArchiveT& archive);
    };

    template<class ObjectContainer, class ValueToPiece, class ValueToID>
    void Scribe<::Arca::Reliquary, BinaryArchive>::JumpSaveAll(
        ObjectT& object,
        ArchiveT& archive,
        ObjectT::KnownPolymorphicSerializerList& fromObject,
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
            auto serializer = FindFrom(id, fromObject)->serializer;
            knownPolymorphics.emplace_back(piece, object, serializer);
            jumpTable.Add(id, knownPolymorphics.back());
        }

        archive(jumpTable);
    }

    template<class FindObject>
    void Scribe<::Arca::Reliquary, BinaryArchive>::JumpLoadAll(
        ObjectT& object,
        ArchiveT& archive,
        ObjectT::KnownPolymorphicSerializerList& fromObject,
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
            const auto& serializer = FindFrom(typeToLoad.arca, fromObject)->serializer;
            auto adapter = KnownPolymorphic(serializedObject, object, serializer);
            jumpTable.FillObject(typeToLoad.inscription, adapter, archive);
        }
    }
}

#include "DynamicRelicDefinition.h"
#include "FixedRelicDefinition.h"