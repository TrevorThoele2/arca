#pragma once

#include "ReliquaryRelics.h"
#include "ReliquaryRelicStructures.h"
#include "ReliquaryShards.h"
#include "ReliquaryCurators.h"
#include "ReliquarySignals.h"

#include "RelicBatch.h"
#include "ShardBatch.h"
#include "SignalBatch.h"
#include "FixedRelic.h"
#include "Static.h"
#include "HasFactoryMethod.h"
#include "StructureFrom.h"

#include "Created.h"
#include "Destroying.h"
#include "RelicParented.h"

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

        void Destroy(const Handle& handle);
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

        void ParentRelicTo(const Handle& parent, const Handle& child);
        std::optional<Handle> ParentOf(const Handle& child);

        [[nodiscard]] SizeT RelicSize() const;
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Ptr<ShardT> Find(RelicID id);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Arca::Batch<ShardT> Batch();

        [[nodiscard]] SizeT ShardSize() const;
    public:
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        Curator* Find(const TypeHandleName& typeHandle);
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] CuratorT* Find();
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] const CuratorT* Find() const;

        [[nodiscard]] SizeT CuratorSize() const;
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(const SignalT& signal);
        template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(Args&& ... args);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void ExecuteOn(const std::function<void(const SignalT&)>& function);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        Arca::Batch<SignalT> Batch();

        [[nodiscard]] SizeT SignalSize() const;
    private:
        using Relics = ReliquaryRelics;
        using RelicStructures = ReliquaryRelicStructures;
        using Shards = ReliquaryShards;
        using Curators = ReliquaryCurators;
        using Signals = ReliquarySignals;
        Relics relics = Relics(*this);
        RelicStructures relicStructures = RelicStructures(*this);
        Shards shards = Shards(*this);
        Curators curators = Curators(*this);
        Signals signals = Signals(*this);
    private:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* FindStorage(RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* FindStaticStorage(RelicID id);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
    private:
        Handle HandleFrom(RelicID id);
        Handle HandleFrom(const RelicMetadata& metadata);
        template<class T>
        Ptr<T> PtrFrom(RelicID id);
        template<class T>
        Ptr<T> PtrFrom(const RelicMetadata& metadata);
    private:
        friend class ReliquaryOrigin;
        friend class ReliquaryComponent;
        friend class ReliquaryRelics;
        friend class ReliquaryRelicStructures;
        friend class ReliquaryShards;
        friend class ReliquaryCurators;
        friend class ReliquarySignals;

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
        const auto id = relics.NextID();

        relics.SetupNewInternals(id, RelicDynamism::Dynamic);

        Raise<Created>(HandleFrom(id));

        return DynamicRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, FixedRelic>, int>>
    FixedRelic Reliquary::Create(const RelicStructure& structure)
    {
        const auto id = relics.NextID();

        relics.SetupNewInternals(id, RelicDynamism::Fixed);
        relics.SatisfyStructure(id, structure);

        Raise<Created>(HandleFrom(id));

        return FixedRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, FixedRelic>, int>>
    FixedRelic Reliquary::Create(const std::string& structureName)
    {
        for (auto& loop : relicStructures.namedList)
            if (loop.name == structureName)
                return Create<FixedRelic>(loop.value);

        throw relicStructures.NotRegistered(structureName);
    }

    template<
        class RelicT,
        class... CreationArgs,
        std::enable_if_t<is_relic_v<RelicT> && has_factory_method_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::Create(CreationArgs&& ... creationArgs)
    {
        const auto id = relics.NextID();
        std::optional<RelicT> relic = Traits<RelicT>::Factory(*this, std::forward<CreationArgs>(creationArgs)...);
        if (!relic)
            return {};
        relic->id = id;

        auto& batchSource = relics.batchSources.Required<RelicT>();
        auto added = batchSource.Add(*relic);

        relics.SetupNewInternals(id, RelicDynamism::Fixed, TypeHandleFor<RelicT>(), added);
        relics.SatisfyStructure(id, StructureFrom<shards_for_t<RelicT>>());
        added->Initialize(*this);

        Raise<Created>(HandleFrom(id));

        return PtrFrom<RelicT>(id);
    }

    template<
        class RelicT,
        class... CreationArgs,
        std::enable_if_t<is_relic_v<RelicT> && !has_factory_method_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::Create(CreationArgs&& ... creationArgs)
    {
        const auto id = relics.NextID();
        RelicT relic(std::forward<CreationArgs>(creationArgs)...);
        relic.id = id;

        auto& batchSource = relics.batchSources.Required<RelicT>();
        auto added = batchSource.Add(relic);

        relics.SetupNewInternals(id, RelicDynamism::Fixed, TypeHandleFor<RelicT>(), added);
        relics.SatisfyStructure(id, StructureFrom<shards_for_t<RelicT>>());
        added->Initialize(*this);

        Raise<Created>(HandleFrom(id));

        return PtrFrom<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void Reliquary::Destroy(RelicT& relic)
    {
        const auto metadata = relics.MetadataFor(relic.ID());
        if (!relics.WillDestroy(metadata))
            return;

        relics.Destroy(*metadata);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, DynamicRelic>, int>>
    std::optional<DynamicRelic> Reliquary::Find(RelicID id)
    {
        const auto metadata = relics.MetadataFor(id);
        if (!metadata || metadata->dynamism != RelicDynamism::Dynamic)
            return {};

        return DynamicRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, FixedRelic>, int>>
    std::optional<FixedRelic> Reliquary::Find(RelicID id)
    {
        const auto metadata = relics.MetadataFor(id);
        if (!metadata || metadata->dynamism != RelicDynamism::Fixed)
            return {};

        return FixedRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::Find(RelicID id)
    {
        const auto metadata = relics.MetadataFor(id);
        if (!metadata)
            return {};

        auto& batchSource = relics.batchSources.Required<RelicT>();
        auto found = batchSource.Find(id);
        if (found == nullptr)
            return {};

        return PtrFrom<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    Static<RelicT> Reliquary::Static()
    {
        const auto typeHandle = TypeHandleFor<RelicT>();
        const auto found = relics.staticMap.find(typeHandle.name);
        if (found == relics.staticMap.end())
            throw relics.NotRegistered(typeHandle, typeid(RelicT));

        auto staticRelic = std::any_cast<RelicT>(&found->second);
        return Arca::Static<RelicT>(staticRelic->ID(), *this);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    Arca::Batch<RelicT> Reliquary::Batch()
    {
        return relics.batchSources.Batch<RelicT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> Reliquary::Find(RelicID id)
    {
        auto& batchSource = shards.batchSources.Required<ShardT>();
        auto found = batchSource.Find(id);
        if (!found)
            return {};

        return Ptr<ShardT>(id, *this);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Arca::Batch<ShardT> Reliquary::Batch()
    {
        return shards.batchSources.Batch<ShardT>();
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    Curator* Reliquary::Find(const TypeHandleName& typeHandle)
    {
        const auto found = curators.map.find(typeHandle);
        if (found == curators.map.end())
            return nullptr;

        return found->second->Get();
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    CuratorT* Reliquary::Find()
    {
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(CuratorT);

        for (auto& loop : curators.map)
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
        return const_cast<Reliquary*>(this)->Find<CuratorT>();
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void Reliquary::Raise(const SignalT& signal)
    {
        auto batchSource = signals.batchSources.Find<SignalT>();
        if (!batchSource)
        {
            const auto typeHandle = TypeHandleFor<SignalT>();
            throw signals.NotRegistered(typeHandle, typeid(SignalT));
        }

        batchSource->Raise(signal);
        signals.ExecuteAllFor(signal);
    }

    template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int>>
    void Reliquary::Raise(Args&& ... args)
    {
        Raise(SignalT{ std::forward<Args>(args)... });
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void Reliquary::ExecuteOn(const std::function<void(const SignalT&)>& function)
    {
        const auto typeHandleName = TypeHandleFor<SignalT>().name;
        auto found = signals.executionMap.find(typeHandleName);
        if (found == signals.executionMap.end())
            found = signals.executionMap.emplace(typeHandleName, Signals::ExecutionList<SignalT>()).first;

        auto& executionList = std::any_cast<Signals::ExecutionList<SignalT>&>(found->second);
        executionList.push_back(function);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    Arca::Batch<SignalT> Reliquary::Batch()
    {
        return signals.batchSources.Batch<SignalT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* Reliquary::FindStorage(RelicID id)
    {
        return relics.FindStorage<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* Reliquary::FindStaticStorage(RelicID id)
    {
        return relics.FindStaticStorage<RelicT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Reliquary::FindStorage(RelicID id)
    {
        return shards.FindStorage<ShardT>(id);
    }

    template<class T>
    Ptr<T> Reliquary::PtrFrom(RelicID id)
    {
        return Ptr<T>(id, *this);
    }

    template<class T>
    Ptr<T> Reliquary::PtrFrom(const RelicMetadata& metadata)
    {
        return Ptr<T>(metadata.id, *this);
    }
}

namespace Inscription
{
    class KnownPolymorphic
    {
    public:
        using Serializer = std::function<void(Arca::Reliquary&, BinaryArchive&)>;
    public:
        explicit KnownPolymorphic(Arca::Reliquary& reliquary, Serializer serializer);
    private:
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
        using KnownPolymorphicSerializerList = Arca::ReliquaryComponent::KnownPolymorphicSerializerList;
    private:
        static void Save(ObjectT& object, ArchiveT& archive);
        static void Load(ObjectT& object, ArchiveT& archive);

        template<class ObjectContainer, class ValueToID>
        static void JumpSaveAll(
            ObjectT& object,
            ArchiveT& archive,
            KnownPolymorphicSerializerList& fromObject,
            ObjectContainer& container,
            ValueToID valueToID);

        static void JumpLoadAll(
            ObjectT& object,
            ArchiveT& archive,
            KnownPolymorphicSerializerList& fromObject);

        static Arca::KnownPolymorphicSerializer* FindFrom(
            TypeHandle mainTypeHandle,
            KnownPolymorphicSerializerList& list);
    private:
        struct TypeHandlePair
        {
            ::Arca::TypeHandleName arca;
            TypeHandle inscription;
        };

        static std::vector<TypeHandlePair> PruneTypesToLoad(
            KnownPolymorphicSerializerList& fromObject,
            ArchiveT& archive,
            const std::vector<TypeHandle>& typeHandlesFromArchive);

        static std::vector<TypeHandlePair> ExtractTypeHandles(
            KnownPolymorphicSerializerList& fromObject,
            ArchiveT& archive);
    };

    template<class ObjectContainer, class ValueToID>
    void Scribe<::Arca::Reliquary, BinaryArchive>::JumpSaveAll(
        ObjectT& object,
        ArchiveT& archive,
        KnownPolymorphicSerializerList& fromObject,
        ObjectContainer& container,
        ValueToID valueToID)
    {
        OutputJumpTable<TypeHandle, KnownPolymorphic> jumpTable;
        std::vector<KnownPolymorphic> knownPolymorphics;
        for (auto& loop : container)
        {
            const auto id = valueToID(loop);
            const auto& serializer = FindFrom(id, fromObject)->serializer;
            knownPolymorphics.emplace_back(object, serializer);
            jumpTable.Add(id, knownPolymorphics.back());
        }

        archive(jumpTable);
    }
}

#include "DynamicRelicDefinition.h"
#include "FixedRelicDefinition.h"
#include "ReliquaryRelicsDefinition.h"
#include "ReliquaryShardsDefinition.h"
#include "ReliquaryCuratorsDefinition.h"
#include "ReliquarySignalsDefinition.h"