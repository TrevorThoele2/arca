#pragma once

#include "ReliquaryRelics.h"
#include "ReliquaryRelicStructures.h"
#include "ReliquaryShards.h"
#include "ReliquaryCurators.h"
#include "ReliquarySignals.h"
#include "ReliquaryMatrices.h"

#include "RelicBatch.h"
#include "ShardBatch.h"
#include "SignalBatch.h"
#include "ClosedRelic.h"

#include "RelicIndex.h"
#include "ShardIndex.h"
#include "GlobalIndex.h"
#include "ComputedIndex.h"
#include "MatrixIndex.h"
#include "AsHandle.h"

#include "Created.h"
#include "Destroying.h"
#include "RelicParented.h"
#include "TransferableSignal.h"
#include "MatrixFormed.h"
#include "MatrixDissolved.h"

#include "Serialization.h"
#include "HasScribe.h"
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
        RelicIndex<RelicT> Create(InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateWith(const RelicStructure& structure, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateWith(const std::string& structureName, InitializeArgs&& ... initializeArgs);

        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateChild(const Handle& parent, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateChildWith(const Handle& parent, const RelicStructure& structure, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateChildWith(const Handle& parent, const std::string& structureName, InitializeArgs&& ... initializeArgs);

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Destroy(RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Destroy(const RelicT& relic);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Destroy(RelicIndex<RelicT> index);

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
        ShardIndex<ShardT> Create(RelicID id);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy(RelicID id);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Arca::Batch<ShardT> Batch() const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const ShardT& shard) const;

        [[nodiscard]] SizeT ShardSize() const;
    public:
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] Arca::Batch<MatrixT> Batch();

        [[nodiscard]] SizeT MatrixSize() const;
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
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && !std::is_same_v<TransferableSignal, SignalT>, int> = 0>
        void Raise(const SignalT& signal);
        template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT> && !std::is_same_v<TransferableSignal, SignalT>, int> = 0>
        void Raise(Args&& ... args);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void ExecuteOn(const std::function<void(const SignalT&)>& function);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        Arca::Batch<SignalT> Batch() const;

        [[nodiscard]] SizeT SignalSize() const;
    public:
        void Destroy(const Handle& handle);
    private:
        using Relics = ReliquaryRelics;
        using RelicStructures = ReliquaryRelicStructures;
        using Shards = ReliquaryShards;
        using Curators = ReliquaryCurators;
        using Signals = ReliquarySignals;
        using Matrices = ReliquaryMatrices;
        Relics relics = Relics(*this);
        RelicStructures relicStructures = RelicStructures(*this);
        Shards shards = Shards(*this);
        Curators curators = Curators(*this);
        Signals signals = Signals(*this);
        Matrices matrices = Matrices(*this);
    private:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ShardIndex<ShardT> CreateFromInternal(RelicID id);
    private:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* FindStorage(RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* FindGlobalStorage();
        template<class T>
        [[nodiscard]] T FindGlobalComputation();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
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
        friend class ReliquaryMatrices;

        friend class OpenRelic;
        friend class ClosedTypedRelic;
        friend class OpenTypedRelic;
        template<class>
        friend class ComputedIndex;
        template<class>
        friend class GlobalIndex;
        template<class>
        friend class MatrixIndex;
        template<class>
        friend class RelicIndex;
        template<class>
        friend class ShardIndex;

        friend class KnownMatrix;
        template<class>
        friend class MatrixImplementation;

        template<class Derived>
        friend class ClosedTypedRelicAutomation;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> Reliquary::Create(InitializeArgs&& ... initializeArgs)
    {
        return relics.Create<RelicT>(std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> Reliquary::CreateWith(const RelicStructure& structure, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateWith<RelicT>(structure, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> Reliquary::CreateWith(const std::string& structureName, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateWith<RelicT>(structureName, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> Reliquary::CreateChild(const Handle& parent, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateChild<RelicT>(parent, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> Reliquary::CreateChildWith(
        const Handle& parent, const RelicStructure& structure, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateChildWith<RelicT>(parent, structure, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> Reliquary::CreateChildWith(
        const Handle& parent, const std::string& structureName, InitializeArgs&& ... initializeArgs)
    {
        return relics.CreateChildWith<RelicT>(parent, structureName, std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void Reliquary::Destroy(RelicID id)
    {
        relics.Destroy<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void Reliquary::Destroy(const RelicT& relic)
    {
        Destroy<RelicT>(relic.ID());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void Reliquary::Destroy(RelicIndex<RelicT> index)
    {
        Destroy<RelicT>(index.ID());
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
        return relics.Batch<RelicT>();
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
    ShardIndex<ShardT> Reliquary::Create(RelicID id)
    {
        return shards.Create<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void Reliquary::Destroy(RelicID id)
    {
        shards.Destroy<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return shards.Contains<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Batch<ShardT> Reliquary::Batch() const
    {
        return shards.Batch<ShardT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    RelicID Reliquary::IDFor(const ShardT& shard) const
    {
        return shards.IDFor(shard);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return matrices.Contains<MatrixT>(id);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    Arca::Batch<MatrixT> Reliquary::Batch()
    {
        return matrices.batchSources.Batch<MatrixT>();
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

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && !std::is_same_v<TransferableSignal, SignalT>, int>>
    void Reliquary::Raise(const SignalT& signal)
    {
        signals.Raise(signal);
    }

    template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT> && !std::is_same_v<TransferableSignal, SignalT>, int>>
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

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardIndex<ShardT> Reliquary::CreateFromInternal(RelicID id)
    {
        return shards.CreateFromInternal<ShardT>(id);
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
    T Reliquary::FindGlobalComputation()
    {
        return relics.FindGlobalComputation<T>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Reliquary::FindStorage(RelicID id)
    {
        return shards.FindStorage<ShardT>(id);
    }
}

namespace Inscription
{
    class KnownPolymorphic
    {
    public:
        using Value = Arca::KnownPolymorphicSerializer*;
    public:
        KnownPolymorphic() = default;
        KnownPolymorphic(Arca::Reliquary& reliquary, Value value);
        KnownPolymorphic(const KnownPolymorphic& arg) = default;

        KnownPolymorphic& operator=(const KnownPolymorphic& arg) = default;
    private:
        Arca::Reliquary* reliquary = nullptr;
        Value value;
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
        template<class T>
        KnownPolymorphicSerializerList ToKnownPolymorphicSerializerList(std::vector<std::unique_ptr<T>>& stored);
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

    template<class T>
    auto Scribe<::Arca::Reliquary, BinaryArchive>::ToKnownPolymorphicSerializerList(std::vector<std::unique_ptr<T>>& stored)
        -> KnownPolymorphicSerializerList
    {
        KnownPolymorphicSerializerList list;
        list.reserve(stored.size());
        for (auto& loop : stored)
            if (loop->WillSerialize())
                list.push_back(loop.get());
        return list;
    }
}

#include "OpenRelicDefinition.h"
#include "ClosedRelicDefinition.h"
#include "OpenTypedRelicDefinition.h"
#include "ReliquaryRelicsDefinition.h"
#include "ReliquaryShardsDefinition.h"
#include "ReliquaryMatricesDefinition.h"
#include "ReliquaryCuratorsDefinition.h"
#include "ReliquarySignalsDefinition.h"
#include "RelicBatchSourceDefinition.h"
#include "MatrixBatchSourceDefinition.h"
#include "TransferableSignalDefinition.h"
#include "KnownMatrixDefinition.h"
#include "EitherDefinition.h"
#include "AllDefinition.h"