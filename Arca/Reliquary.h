#pragma once

#include "ReliquaryRelics.h"
#include "ReliquaryRelicStructures.h"
#include "ReliquaryShards.h"
#include "ReliquaryCurators.h"
#include "ReliquarySignals.h"

#include "RelicBatch.h"
#include "ShardBatch.h"
#include "EitherShardBatch.h"
#include "CompositeShardBatch.h"
#include "SignalBatch.h"
#include "ClosedRelic.h"
#include "HasFactoryMethod.h"
#include "StructureFrom.h"
#include "Either.h"
#include "AreAllShards.h"
#include "Global.h"

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
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, OpenRelic>, int> = 0>
        OpenRelic Create();
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int> = 0>
        ClosedRelic Create(const RelicStructure& structure);
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int> = 0>
        ClosedRelic Create(const std::string& structureName);
        template<
            class RelicT,
            class... CreationArgs,
            std::enable_if_t<is_relic_v<RelicT> && has_factory_method_v<RelicT>, int> = 0>
        LocalPtr<RelicT> Create(CreationArgs&& ... creationArgs);
        template<
            class RelicT,
            class... CreationArgs,
            std::enable_if_t<is_relic_v<RelicT> && !has_factory_method_v<RelicT>, int> = 0>
        LocalPtr<RelicT> Create(CreationArgs&& ... creationArgs);

        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, OpenRelic>, int> = 0>
        OpenRelic CreateChild(const Handle& parent);
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int> = 0>
        ClosedRelic CreateChild(const Handle& parent, const RelicStructure& structure);
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int> = 0>
        ClosedRelic CreateChild(const Handle& parent, const std::string& structureName);
        template<
            class RelicT,
            class... CreationArgs,
            std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        LocalPtr<RelicT> CreateChild(const Handle& parent, CreationArgs&& ... creationArgs);

        void Destroy(const Handle& handle);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Destroy(RelicT& relic);

        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, OpenRelic>, int> = 0>
        [[nodiscard]] std::optional<OpenRelic> Find(RelicID id) const;
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int> = 0>
        [[nodiscard]] std::optional<ClosedRelic> Find(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] LocalPtr<RelicT> Find(RelicID id) const;
        template<class GlobalT, std::enable_if_t<is_global_v<GlobalT>, int> = 0>
        [[nodiscard]] GlobalPtr<typename GlobalT::RelicT> Find() const;

        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, OpenRelic>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Arca::Batch<RelicT> Batch() const;

        [[nodiscard]] std::optional<Handle> ParentOf(const Handle& child) const;

        [[nodiscard]] std::vector<RelicID> AllIDs() const;

        [[nodiscard]] SizeT RelicSize() const;
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] LocalPtr<ShardT> Find(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class... ShardsT, std::enable_if_t<are_all_shards_v<ShardsT...> && (sizeof...(ShardsT) > 1), int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Arca::Batch<ShardT> Batch() const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] Arca::Batch<EitherT> Batch();
        template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int> = 0>
        [[nodiscard]] Arca::Batch<ShardsT> Batch();

        [[nodiscard]] SizeT ShardSize() const;
    public:
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] Curator* Find(const TypeHandleName& typeHandle);
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] const Curator* Find(const TypeHandleName& typeHandle) const;
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] CuratorT* Find();
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] const CuratorT* Find() const;

        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] bool Contains(const TypeHandleName& typeHandle) const;
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] bool Contains() const;

        [[nodiscard]] SizeT CuratorSize() const;
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(const SignalT& signal);
        template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(Args&& ... args);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void ExecuteOn(const std::function<void(const SignalT&)>& function);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        Arca::Batch<SignalT> Batch() const;

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
        [[nodiscard]] RelicT* FindGlobalStorage();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
    private:
        Handle HandleFrom(RelicID id);
        Handle HandleFrom(const RelicMetadata& metadata);
        template<class T>
        LocalPtr<T> LocalPtrFrom(RelicID id) const;
        template<class T>
        LocalPtr<T> LocalPtrFrom(const RelicMetadata& metadata) const;
        template<class T>
        GlobalPtr<T> GlobalPtrFrom(RelicID id) const;
    private:
        friend class ReliquaryOrigin;
        friend class ReliquaryComponent;
        friend class ReliquaryRelics;
        friend class ReliquaryRelicStructures;
        friend class ReliquaryShards;
        friend class ReliquaryCurators;
        friend class ReliquarySignals;

        friend class OpenRelic;
        friend class TypedRelic;
        template<class>
        friend class LocalPtr;
        template<class>
        friend class GlobalPtr;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, OpenRelic>, int>>
    OpenRelic Reliquary::Create()
    {
        const auto id = relics.AdvanceID();

        relics.SetupNewInternals(id, RelicOpenness::Open);
        shards.NotifyCompositesRelicCreate(id, {});

        Raise<Created>(HandleFrom(id));

        return OpenRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int>>
    ClosedRelic Reliquary::Create(const RelicStructure& structure)
    {
        const auto id = relics.AdvanceID();

        relics.SetupNewInternals(id, RelicOpenness::Closed);
        relics.SatisfyStructure(id, structure);
        shards.NotifyCompositesRelicCreate(id, structure);

        Raise<Created>(HandleFrom(id));

        return ClosedRelic(id, *this);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int>>
    ClosedRelic Reliquary::Create(const std::string& structureName)
    {
        for (auto& loop : relicStructures.namedList)
            if (loop.name == structureName)
                return Create<ClosedRelic>(loop.value);

        throw relicStructures.NotRegistered(structureName);
    }

    template<
        class RelicT,
        class... CreationArgs,
        std::enable_if_t<is_relic_v<RelicT> && has_factory_method_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::Create(CreationArgs&& ... creationArgs)
    {
        const auto id = relics.AdvanceID();
        std::optional<RelicT> relic = Traits<RelicT>::Factory(*this, std::forward<CreationArgs>(creationArgs)...);
        if (!relic)
            return {};
        relic->id = id;

        auto& batchSource = relics.batchSources.Required<RelicT>();
        auto added = batchSource.Add(*relic);

        auto structure = StructureFrom<shards_for_t<RelicT>>();
        relics.SetupNewInternals(id, RelicOpenness::Typed, TypeHandleFor<RelicT>(), added);
        relics.SatisfyStructure(id, structure);
        added->Initialize(*this);
        shards.NotifyCompositesRelicCreate(id, structure);

        Raise<Created>(HandleFrom(id));

        return LocalPtrFrom<RelicT>(id);
    }

    template<
        class RelicT,
        class... CreationArgs,
        std::enable_if_t<is_relic_v<RelicT> && !has_factory_method_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::Create(CreationArgs&& ... creationArgs)
    {
        const auto id = relics.AdvanceID();
        RelicT relic(std::forward<CreationArgs>(creationArgs)...);
        relic.id = id;

        auto& batchSource = relics.batchSources.Required<RelicT>();
        auto added = batchSource.Add(relic);

        auto structure = StructureFrom<shards_for_t<RelicT>>();
        relics.SetupNewInternals(id, RelicOpenness::Typed, TypeHandleFor<RelicT>(), added);
        relics.SatisfyStructure(id, structure);
        added->Initialize(*this);
        shards.NotifyCompositesRelicCreate(id, structure);

        Raise<Created>(HandleFrom(id));

        return LocalPtrFrom<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, OpenRelic>, int>>
    OpenRelic Reliquary::CreateChild(const Handle& parent)
    {
        relics.ThrowIfCannotParent(parent, Relics::RelicPrototype{ relics.NextID(), RelicOpenness::Open });
        auto child = Create<RelicT>();
        relics.Parent(parent, child);
        return child;
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int>>
    ClosedRelic Reliquary::CreateChild(const Handle& parent, const RelicStructure& structure)
    {
        relics.ThrowIfCannotParent(parent, Relics::RelicPrototype{ relics.NextID(), RelicOpenness::Closed });
        auto child = Create<RelicT>(structure);
        relics.Parent(parent, child);
        return child;
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int>>
    ClosedRelic Reliquary::CreateChild(const Handle& parent, const std::string& structureName)
    {
        relics.ThrowIfCannotParent(parent, Relics::RelicPrototype{ relics.NextID(), RelicOpenness::Closed });
        auto child = Create<RelicT>(structureName);
        relics.Parent(parent, child);
        return child;
    }

    template<
        class RelicT,
        class... CreationArgs,
        std::enable_if_t<is_relic_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::CreateChild(const Handle& parent, CreationArgs&& ... creationArgs)
    {
        relics.ThrowIfCannotParent(parent, Relics::RelicPrototype{ relics.NextID(), RelicOpenness::Typed });
        auto child = Create<RelicT>(std::forward<CreationArgs>(creationArgs)...);
        relics.Parent(parent, child);
        return child;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void Reliquary::Destroy(RelicT& relic)
    {
        const auto metadata = relics.MetadataFor(relic.ID());
        if (!relics.WillDestroy(metadata))
            return;

        relics.Destroy(*metadata);
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, OpenRelic>, int>>
    std::optional<OpenRelic> Reliquary::Find(RelicID id) const
    {
        const auto metadata = relics.MetadataFor(id);
        if (!metadata || metadata->openness != RelicOpenness::Open)
            return {};

        return OpenRelic(id, const_cast<Reliquary&>(*this));
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int>>
    std::optional<ClosedRelic> Reliquary::Find(RelicID id) const
    {
        const auto metadata = relics.MetadataFor(id);
        if (!metadata || metadata->openness != RelicOpenness::Closed)
            return {};

        return ClosedRelic(id, const_cast<Reliquary&>(*this));
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::Find(RelicID id) const
    {
        const auto metadata = relics.MetadataFor(id);
        if (!metadata || metadata->openness != RelicOpenness::Typed)
            return {};

        auto& batchSource = relics.batchSources.Required<RelicT>();
        auto found = batchSource.Find(id);
        if (found == nullptr)
            return {};

        return LocalPtrFrom<RelicT>(id);
    }

    template<class GlobalT, std::enable_if_t<is_global_v<GlobalT>, int>>
    GlobalPtr<typename GlobalT::RelicT> Reliquary::Find() const
    {
        using RelicT = typename GlobalT::RelicT;

        const auto typeHandle = TypeHandleFor<RelicT>();
        const auto found = relics.globalMap.find(typeHandle.name);
        if (found == relics.globalMap.end())
            throw relics.NotRegistered(typeHandle, typeid(RelicT));

        auto relic = std::any_cast<RelicT>(&found->second);
        return GlobalPtrFrom<RelicT>(relic->ID());
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, OpenRelic>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return static_cast<bool>(Find<RelicT>(id));
    }

    template<class RelicT, std::enable_if_t<std::is_same_v<RelicT, ClosedRelic>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return static_cast<bool>(Find<RelicT>(id));
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return static_cast<bool>(Find<RelicT>(id));
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    Arca::Batch<RelicT> Reliquary::Batch() const
    {
        return relics.batchSources.Batch<RelicT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    LocalPtr<ShardT> Reliquary::Find(RelicID id) const
    {
        return shards.Find<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return shards.Contains<ShardT>(id);
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return shards.Contains<EitherT>(id);
    }

    template<class... ShardsT, std::enable_if_t<are_all_shards_v<ShardsT...> && (sizeof...(ShardsT) > 1), int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return shards.Contains<ShardsT...>(id);
    }

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return shards.Contains<ShardsT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Arca::Batch<ShardT> Reliquary::Batch() const
    {
        return shards.batchSources.Batch<ShardT>();
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    Arca::Batch<EitherT> Reliquary::Batch()
    {
        return shards.eitherBatchSources.Batch<EitherT>();
    }

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    Arca::Batch<ShardsT> Reliquary::Batch()
    {
        return shards.compositeBatchSources.Batch<ShardsT>();
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    Curator* Reliquary::Find(const TypeHandleName& typeHandle)
    {
        const auto found = curators.map.find(typeHandle);
        if (found == curators.map.end())
            return nullptr;

        return found->second->Get();
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    const Curator* Reliquary::Find(const TypeHandleName& typeHandle) const
    {
        return const_cast<Reliquary&>(*this).Find<CuratorT>(typeHandle);
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

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    bool Reliquary::Contains(const TypeHandleName& typeHandle) const
    {
        return static_cast<bool>(Find<CuratorT>(typeHandle));
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    bool Reliquary::Contains() const
    {
        return static_cast<bool>(Find<CuratorT>());
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
    Arca::Batch<SignalT> Reliquary::Batch() const
    {
        return signals.batchSources.Batch<SignalT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* Reliquary::FindStorage(RelicID id)
    {
        return relics.FindStorage<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* Reliquary::FindGlobalStorage()
    {
        return relics.FindGlobalStorage<RelicT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Reliquary::FindStorage(RelicID id)
    {
        return shards.FindStorage<ShardT>(id);
    }

    template<class T>
    LocalPtr<T> Reliquary::LocalPtrFrom(RelicID id) const
    {
        return LocalPtr<T>(id, const_cast<Reliquary&>(*this));
    }

    template<class T>
    LocalPtr<T> Reliquary::LocalPtrFrom(const RelicMetadata& metadata) const
    {
        return LocalPtr<T>(metadata.id, const_cast<Reliquary&>(*this));
    }

    template<class T>
    GlobalPtr<T> Reliquary::GlobalPtrFrom(RelicID id) const
    {
        return GlobalPtr<T>(id, const_cast<Reliquary&>(*this));
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

#include "OpenRelicDefinition.h"
#include "ClosedRelicDefinition.h"
#include "ReliquaryRelicsDefinition.h"
#include "ReliquaryShardsDefinition.h"
#include "ReliquaryCuratorsDefinition.h"
#include "ReliquarySignalsDefinition.h"
#include "CompositeShardBatchSourceDefinition.h"