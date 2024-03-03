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
#include "StructureFrom.h"
#include "Either.h"
#include "AreAllShards.h"

#include "Destroying.h"
#include "RelicParented.h"

#include "Serialization.h"
#include "TypeHandleSerialization.h"
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
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> Create(CreationArgs&& ... creationArgs);
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateWith(const RelicStructure& structure, CreationArgs&& ... creationArgs);
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateWith(const std::string& structureName, CreationArgs&& ... creationArgs);

        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateChild(const Handle& parent, CreationArgs&& ... creationArgs);
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateChildWith(const Handle& parent, const RelicStructure& structure, CreationArgs&& ... creationArgs);
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateChildWith(const Handle& parent, const std::string& structureName, CreationArgs&& ... creationArgs);

        void Destroy(const Handle& handle);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Destroy(RelicT& relic);

        template<class RelicT, std::enable_if_t<is_local_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Ptr<RelicT> Find(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_global_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Ptr<RelicT> Find() const;

        template<class RelicT, std::enable_if_t<is_local_relic_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_global_relic_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains() const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Arca::Batch<RelicT> Batch() const;

        [[nodiscard]] std::optional<Handle> ParentOf(const Handle& child) const;

        [[nodiscard]] std::vector<RelicID> AllIDs() const;

        [[nodiscard]] SizeT RelicSize() const;
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Ptr<ShardT> Find(RelicID id) const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] Ptr<EitherT> Find(RelicID id) const;

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
        Handle HandleFrom(RelicID id, TypeHandle typeHandle);
        Handle HandleFrom(const RelicMetadata& metadata);
        template<class T>
        Ptr<T> PtrFrom(RelicID id) const;
        template<class T>
        Ptr<T> PtrFrom(const RelicMetadata& metadata) const;
    private:
        friend class ReliquaryOrigin;
        friend class ReliquaryComponent;
        friend class ReliquaryRelics;
        friend class ReliquaryRelicStructures;
        friend class ReliquaryShards;
        friend class ReliquaryCurators;
        friend class ReliquarySignals;

        friend class OpenRelic;
        friend class ClosedTypedRelic;
        friend class OpenTypedRelic;
        template<class, class>
        friend class Ptr;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::Create(CreationArgs&& ... creationArgs)
    {
        return relics.Create<RelicT>(std::forward<CreationArgs>(creationArgs)...);
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::CreateWith(const RelicStructure& structure, CreationArgs&& ... creationArgs)
    {
        return relics.CreateWith<RelicT>(structure, std::forward<CreationArgs>(creationArgs)...);
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::CreateWith(const std::string& structureName, CreationArgs&& ... creationArgs)
    {
        return relics.CreateWith<RelicT>(structureName, std::forward<CreationArgs>(creationArgs)...);
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::CreateChild(const Handle& parent, CreationArgs&& ... creationArgs)
    {
        return relics.CreateChild<RelicT>(parent, std::forward<CreationArgs>(creationArgs)...);
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::CreateChildWith(
        const Handle& parent, const RelicStructure& structure, CreationArgs&& ... creationArgs)
    {
        return relics.CreateChildWith<RelicT>(parent, structure, std::forward<CreationArgs>(creationArgs)...);
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::CreateChildWith(
        const Handle& parent, const std::string& structureName, CreationArgs&& ... creationArgs)
    {
        return relics.CreateChildWith<RelicT>(parent, structureName, std::forward<CreationArgs>(creationArgs)...);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void Reliquary::Destroy(RelicT& relic)
    {
        const auto metadata = relics.MetadataFor(relic.ID());
        if (!relics.WillDestroy(metadata))
            return;

        relics.Destroy(*metadata);
    }

    template<class RelicT, std::enable_if_t<is_local_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::Find(RelicID id) const
    {
        return relics.Find<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_global_relic_v<RelicT>, int>>
    Ptr<RelicT> Reliquary::Find() const
    {
        return relics.Find<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_local_relic_v<RelicT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return static_cast<bool>(Find<RelicT>(id));
    }

    template<class RelicT, std::enable_if_t<is_global_relic_v<RelicT>, int>>
    bool Reliquary::Contains() const
    {
        return static_cast<bool>(Find<RelicT>());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    Arca::Batch<RelicT> Reliquary::Batch() const
    {
        return relics.batchSources.Batch<RelicT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> Reliquary::Find(RelicID id) const
    {
        return shards.Find<ShardT>(id);
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    Ptr<EitherT> Reliquary::Find(RelicID id) const
    {
        return shards.Find<EitherT>(id);
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
    Ptr<T> Reliquary::PtrFrom(RelicID id) const
    {
        return Ptr<T>(id, const_cast<Reliquary&>(*this));
    }

    template<class T>
    Ptr<T> Reliquary::PtrFrom(const RelicMetadata& metadata) const
    {
        return Ptr<T>(metadata.id, const_cast<Reliquary&>(*this));
    }
}

namespace Inscription
{
    class KnownPolymorphic
    {
    public:
        using Serializer = std::function<void(Arca::Reliquary&, BinaryArchive&)>;
    public:
        KnownPolymorphic(Arca::Reliquary& reliquary, Serializer serializer);
        KnownPolymorphic(const KnownPolymorphic& arg) = default;

        KnownPolymorphic& operator=(const KnownPolymorphic& arg) = default;
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
        void Save(ObjectT& object, ArchiveT& archive);
        void Load(ObjectT& object, ArchiveT& archive);

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
        struct LoadedRelicMetadata
        {
            Arca::RelicID id = 0;
            Arca::Openness openness = Arca::Openness::Open;

            std::optional<Arca::RelicID> parent;
            std::vector<Arca::RelicID> children;
        };
        std::vector<LoadedRelicMetadata> loadedRelicMetadata;

        static void SaveRelicMetadata(Arca::RelicMetadata& metadata, ArchiveT& archive);
        static LoadedRelicMetadata LoadRelicMetadata(ObjectT& object, ArchiveT& archive);

        using MetadataExtension = std::tuple<Arca::TypeHandle, Arca::Locality, void*>;
        static MetadataExtension FindExtensionForLoadedMetadata(Arca::RelicID id, ObjectT& object);
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
        knownPolymorphics.reserve(container.size());
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
#include "OpenTypedRelicDefinition.h"
#include "ReliquaryRelicsDefinition.h"
#include "ReliquaryShardsDefinition.h"
#include "ReliquaryCuratorsDefinition.h"
#include "ReliquarySignalsDefinition.h"
#include "CompositeShardBatchSourceDefinition.h"