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

#include "LocalPtr.h"
#include "AsHandle.h"

#include "Created.h"
#include "Destroying.h"
#include "RelicParented.h"

#include "Serialization.h"
#include "TypeScribe.h"
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
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        LocalPtr<RelicT> Create(InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        LocalPtr<RelicT> CreateWith(const RelicStructure& structure, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        LocalPtr<RelicT> CreateWith(const std::string& structureName, InitializeArgs&& ... initializeArgs);

        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        LocalPtr<RelicT> CreateChild(const Handle& parent, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        LocalPtr<RelicT> CreateChildWith(const Handle& parent, const RelicStructure& structure, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        LocalPtr<RelicT> CreateChildWith(const Handle& parent, const std::string& structureName, InitializeArgs&& ... initializeArgs);

        void Clear(const Type& type);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Clear();

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains() const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Batch<RelicT> Batch() const;

        [[nodiscard]] std::optional<Handle> ParentOf(const Handle& child) const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const RelicT& relic) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicID IDFor() const;

        [[nodiscard]] std::vector<RelicID> AllIDs() const;

        [[nodiscard]] SizeT RelicSize() const;
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Arca::Batch<ShardT> Batch() const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] Arca::Batch<EitherT> Batch();
        template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int> = 0>
        [[nodiscard]] Arca::Batch<ShardsT> Batch();

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const ShardT& shard) const;

        [[nodiscard]] SizeT ShardSize() const;
    public:
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] Curator& Find(const TypeName& type);
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] const Curator& Find(const TypeName& type) const;
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] CuratorT& Find();
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] const CuratorT& Find() const;

        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] bool Contains(const TypeName& type) const;
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
    public:
        void Destroy(const Handle& handle);
        template<class T>
        void Destroy(const T& object);
        template<class T>
        void Destroy(RelicID id);
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
        template<class T>
        [[nodiscard]] T FindGlobalAliasStorage();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] typename EitherT::ShardT* FindStorage(RelicID id);
    private:
        Handle HandleFrom(RelicID id, Type type, HandleObjectType objectType);
        Handle HandleFrom(const RelicMetadata& metadata);
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
        template<class>
        friend class LocalPtr;
        template<class>
        friend class GlobalPtr;
        template<class>
        friend class AliasPtr;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::Create(InitializeArgs&& ... initializeArgs)
    {
        return relics.Create<RelicT>(std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::CreateWith(const RelicStructure& structure, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateWith<RelicT>(structure, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::CreateWith(const std::string& structureName, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateWith<RelicT>(structureName, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::CreateChild(const Handle& parent, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateChild<RelicT>(parent, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::CreateChildWith(
        const Handle& parent, const RelicStructure& structure, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateChildWith<RelicT>(parent, structure, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    LocalPtr<RelicT> Reliquary::CreateChildWith(
        const Handle& parent, const std::string& structureName, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateChildWith<RelicT>(parent, structureName, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void Reliquary::Clear()
    {
        relics.Clear<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return relics.Contains<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    bool Reliquary::Contains() const
    {
        return relics.Contains<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    Batch<RelicT> Reliquary::Batch() const
    {
        return relics.batchSources.Batch<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicID Reliquary::IDFor(const RelicT& relic) const
    {
        return relics.IDFor(relic);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicID Reliquary::IDFor() const
    {
        return relics.IDFor<RelicT>();
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

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return shards.Contains<ShardsT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Batch<ShardT> Reliquary::Batch() const
    {
        return shards.batchSources.Batch<ShardT>();
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    Batch<EitherT> Reliquary::Batch()
    {
        return shards.eitherBatchSources.Batch<EitherT>();
    }

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    Batch<ShardsT> Reliquary::Batch()
    {
        return shards.compositeBatchSources.Batch<ShardsT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    RelicID Reliquary::IDFor(const ShardT& shard) const
    {
        return shards.IDFor(shard);
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    Curator& Reliquary::Find(const TypeName& type)
    {
        return curators.Find<CuratorT>(type);
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    const Curator& Reliquary::Find(const TypeName& type) const
    {
        return curators.Find<CuratorT>(type);
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    CuratorT& Reliquary::Find()
    {
        return curators.Find<CuratorT>();
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    const CuratorT& Reliquary::Find() const
    {
        return curators.Find<CuratorT>();
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    bool Reliquary::Contains(const TypeName& type) const
    {
        return static_cast<bool>(Find<CuratorT>(type));
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    bool Reliquary::Contains() const
    {
        return static_cast<bool>(Find<CuratorT>());
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void Reliquary::Raise(const SignalT& signal)
    {
        signals.Raise(signal);
    }

    template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int>>
    void Reliquary::Raise(Args&& ... args)
    {
        signals.Raise<SignalT>(std::forward<Args>(args)...);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void Reliquary::ExecuteOn(const std::function<void(const SignalT&)>& function)
    {
        signals.ExecuteOn(function);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    Batch<SignalT> Reliquary::Batch() const
    {
        return signals.batchSources.Batch<SignalT>();
    }

    template<class T>
    void Reliquary::Destroy(const T& object)
    {
        Destroy(AsHandle(object));
    }

    template<class T>
    void Reliquary::Destroy(RelicID id)
    {
        Destroy(AsHandle<T>(id, *this));
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

    template<class T>
    T Reliquary::FindGlobalAliasStorage()
    {
        return relics.FindGlobalAliasStorage<T>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Reliquary::FindStorage(RelicID id)
    {
        return shards.FindStorage<ShardT>(id);
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    typename EitherT::ShardT* Reliquary::FindStorage(RelicID id)
    {
        return shards.FindStorage<EitherT>(id);
    }
}

namespace Inscription
{
    class KnownPolymorphic
    {
    public:
        using Serializer = std::function<void(Arca::Reliquary&, BinaryArchive&)>;
    public:
        KnownPolymorphic() = default;
        KnownPolymorphic(Arca::Reliquary& reliquary, Serializer serializer);
        KnownPolymorphic(const KnownPolymorphic& arg) = default;

        KnownPolymorphic& operator=(const KnownPolymorphic& arg) = default;
    private:
        Arca::Reliquary* reliquary = nullptr;
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

        static void JumpSaveAll(
            ObjectT& object,
            ArchiveT& archive,
            KnownPolymorphicSerializerList& polymorphicsFromObject);

        static void JumpLoadAll(
            ObjectT& object,
            ArchiveT& archive,
            KnownPolymorphicSerializerList& polymorphicsFromObject);

        static Arca::KnownPolymorphicSerializer* FindFrom(
            Type mainType,
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

        using MetadataExtension = std::tuple<Arca::Type, Arca::Locality, void*>;
        static MetadataExtension FindExtensionForLoadedMetadata(Arca::RelicID id, ObjectT& object);
    private:
        struct TypePair
        {
            ::Arca::TypeName arca;
            Type inscription;
        };

        static std::vector<TypePair> PruneTypesToLoad(
            KnownPolymorphicSerializerList& fromObject,
            ArchiveT& archive,
            const std::vector<Type>& typesFromArchive);

        static std::vector<TypePair> ExtractTypes(
            KnownPolymorphicSerializerList& fromObject,
            ArchiveT& archive);
    };
}

#include "OpenRelicDefinition.h"
#include "ClosedRelicDefinition.h"
#include "OpenTypedRelicDefinition.h"
#include "ReliquaryRelicsDefinition.h"
#include "ReliquaryShardsDefinition.h"
#include "ReliquaryCuratorsDefinition.h"
#include "ReliquarySignalsDefinition.h"
#include "RelicBatchSourceDefinition.h"
#include "CompositeShardBatchSourceDefinition.h"