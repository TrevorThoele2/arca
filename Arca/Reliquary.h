#pragma once

#include "ReliquaryRelics.h"
#include "ReliquaryRelicStructures.h"
#include "ReliquaryShards.h"
#include "ReliquaryMatrices.h"
#include "ReliquaryCurators.h"
#include "ReliquaryCommands.h"
#include "ReliquarySignals.h"

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

#include "Create.h"
#include "CreateWith.h"
#include "CreateChild.h"
#include "CreateChildWith.h"
#include "Destroy.h"
#include "Clear.h"

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
        template<class CommandT, std::enable_if_t<is_command_v<CommandT> && std::is_void_v<command_return_t<CommandT>>, int> = 0>
        void Do(const CommandT& command);
        template<class CommandT, class... Args, std::enable_if_t<is_command_v<CommandT> && std::is_void_v<command_return_t<CommandT>>, int> = 0>
        void Do(Args&& ... args);

        template<class CommandT, std::enable_if_t<is_command_v<CommandT> && !std::is_void_v<command_return_t<CommandT>>, int> = 0>
        command_return_t<CommandT> Do(const CommandT& command);
        template<class CommandT, class... Args, std::enable_if_t<is_command_v<CommandT> && !std::is_void_v<command_return_t<CommandT>>, int> = 0>
        command_return_t<CommandT> Do(Args&& ... args);
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
        using Matrices = ReliquaryMatrices;
        using Curators = ReliquaryCurators;
        using Commands = ReliquaryCommands;
        using Signals = ReliquarySignals;
        Relics relics = Relics(*this);
        RelicStructures relicStructures = RelicStructures(*this);
        Shards shards = Shards(*this);
        Matrices matrices = Matrices(*this);
        Curators curators = Curators(*this);
        Commands commands = Commands(*this);
        Signals signals = Signals(*this);
    private:
        template<class ShardT, class... InitializeArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ShardIndex<ShardT> CreateFromInternal(RelicID id, InitializeArgs&& ... initializeArgs);
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
        friend class ReliquaryMatrices;
        friend class ReliquaryCurators;
        friend class ReliquaryCommands;
        friend class ReliquarySignals;

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
        template<class, class>
        friend class ShardIndex;

        friend class KnownMatrix;
        template<class>
        friend class MatrixImplementation;

        template<class Derived>
        friend class ClosedTypedRelicAutomation;

        template<class, class>
        friend class BatchSource;

        friend class Curator;
    private:
        INSCRIPTION_ACCESS;
    };

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

    template<class CommandT, std::enable_if_t<is_command_v<CommandT> && std::is_void_v<command_return_t<CommandT>>, int>>
    void Reliquary::Do(const CommandT& command)
    {
        commands.Do(command);
    }

    template<class CommandT, class... Args, std::enable_if_t<is_command_v<CommandT> && std::is_void_v<command_return_t<CommandT>>, int>>
    void Reliquary::Do(Args&& ... args)
    {
        Do(CommandT(std::forward<Args>(args)...));
    }

    template<class CommandT, std::enable_if_t<is_command_v<CommandT> && !std::is_void_v<command_return_t<CommandT>>, int>>
    command_return_t<CommandT> Reliquary::Do(const CommandT& command)
    {
        return commands.Do(command);
    }

    template<class CommandT, class... Args, std::enable_if_t<is_command_v<CommandT> && !std::is_void_v<command_return_t<CommandT>>, int>>
    command_return_t<CommandT> Reliquary::Do(Args&& ... args)
    {
        return Do(CommandT(std::forward<Args>(args)...));
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

    template<class ShardT, class... InitializeArgs, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardIndex<ShardT> Reliquary::CreateFromInternal(RelicID id, InitializeArgs&& ... initializeArgs)
    {
        return shards.CreateFromInternal<ShardT>(id, std::forward<InitializeArgs>(initializeArgs)...);
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
#include "ReliquaryCommandsDefinition.h"

#include "RelicBatchSourceDefinition.h"
#include "MatrixBatchSourceDefinition.h"
#include "TransferableSignalDefinition.h"
#include "KnownMatrixDefinition.h"
#include "EitherDefinition.h"
#include "AllDefinition.h"