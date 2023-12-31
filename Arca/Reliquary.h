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

#include "RelicIndex.h"
#include "ShardIndex.h"
#include "GlobalIndex.h"
#include "MatrixIndex.h"
#include "AsHandle.h"
#include "IsSignal.h"

#include "ReliquaryUserContext.h"
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
        Reliquary();
        Reliquary(const Reliquary& arg) = delete;
        Reliquary(Reliquary&& arg) noexcept = delete;
        ~Reliquary() = default;
        Reliquary& operator=(const Reliquary& arg) = delete;
        Reliquary& operator=(Reliquary&& arg) noexcept = delete;
    public:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_local_v<RelicT>, int> = 0>
        [[nodiscard]] Index<RelicT> Find(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_global_v<RelicT>, int> = 0>
        [[nodiscard]] Index<RelicT> Find() const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains() const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Batch<RelicT> Batch() const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const RelicT& relic) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicID IDFor() const;

        [[nodiscard]] bool IsRelicTypeName(const TypeName& typeName) const;
        [[nodiscard]] std::vector<TypeName> AllRelicTypeNames() const;

        [[nodiscard]] std::vector<RelicID> AllIDs() const;

        [[nodiscard]] SizeT RelicSize() const;
        [[nodiscard]] SizeT RelicSize(const TypeName& typeName) const;

        [[nodiscard]] RelicID NextRelicID() const;
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Index<ShardT> Find(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Arca::Batch<ShardT> Batch() const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const ShardT& shard) const;

        [[nodiscard]] bool IsShardTypeName(const TypeName& typeName) const;
        [[nodiscard]] std::vector<TypeName> AllShardTypeNames() const;

        [[nodiscard]] SizeT ShardSize() const;
        [[nodiscard]] SizeT ShardSize(const Type& type) const;
    public:
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] Index<MatrixT> Find(RelicID id) const;

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
        template<class CommandT, std::enable_if_t<is_command_v<CommandT> && !has_command_result_v<CommandT>, int> = 0>
        void Do(const CommandT& command);

        template<class CommandT, std::enable_if_t<is_command_v<CommandT> && has_command_result_v<CommandT>, int> = 0>
        command_result_t<CommandT> Do(const CommandT& command);
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(const SignalT& signal);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && !is_matrix_signal_v<SignalT>, int> = 0>
        void On(const std::function<void(const SignalT&)>&function);
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && is_matrix_signal_v<SignalT>, int> = 0>
        void On(const std::function<void(const SignalT&)>& function);
    public:
        void Destroy(const Handle& handle);
    private:
        using RelicStructures = ReliquaryRelicStructures;
        RelicStructures relicStructures;
        using Signals = ReliquarySignals;
        Signals signals;
        using Curators = ReliquaryCurators;
        Curators curators;
        using Matrices = ReliquaryMatrices;
        Matrices matrices;
        using Commands = ReliquaryCommands;
        Commands commands;
        using Shards = ReliquaryShards;
        Shards shards;
        using Relics = ReliquaryRelics;
        Relics relics;
    private:
        template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Index<ShardT> CreateFromInternal(RelicID id, ConstructorArgs&& ... constructorArgs);
    private:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* FindStorage(RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* FindGlobalStorage();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
    private:
        Handle HandleFrom(const RelicMetadata& metadata);
    private:
        friend class ReliquaryOrigin;
        
        template<class, class>
        friend class Index;

        friend class KnownMatrix;
        template<class>
        friend class MatrixImplementation;

        template<class Derived>
        friend class ClosedTypedRelic;

        template<class, class>
        friend class BatchSource;

        friend class Curator;
        friend class MutablePointer;

        template<class, class>
        friend struct Create;
        template<class, class>
        friend struct Destroy;
        friend struct Clear;
        template<class, class>
        friend struct AssignCopy;
        template<class, class>
        friend struct AssignMove;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_local_v<RelicT>, int>>
    Index<RelicT> Reliquary::Find(RelicID id) const
    {
        return relics.Find<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_global_v<RelicT>, int>>
    Index<RelicT> Reliquary::Find() const
    {
        return relics.Find<RelicT>();
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
    Index<ShardT> Reliquary::Find(RelicID id) const
    {
        return shards.Find<ShardT>(id);
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
    Index<MatrixT> Reliquary::Find(RelicID id) const
    {
        auto& self = const_cast<Reliquary&>(*this);
        return { id, self, MatrixImplementation<MatrixT>::CreateIndexValue(id, self) };
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    bool Reliquary::Contains(RelicID id) const
    {
        return shards.Contains<MatrixT>(id);
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
        return curators.Contains<CuratorT>(type);
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    bool Reliquary::Contains() const
    {
        return curators.Contains<CuratorT>();
    }

    template<class CommandT, std::enable_if_t<is_command_v<CommandT> && !has_command_result_v<CommandT>, int>>
    void Reliquary::Do(const CommandT& command)
    {
        commands.Do(command);
    }

    template<class CommandT, std::enable_if_t<is_command_v<CommandT> && has_command_result_v<CommandT>, int>>
    command_result_t<CommandT> Reliquary::Do(const CommandT& command)
    {
        return commands.Do(command);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void Reliquary::Raise(const SignalT& signal)
    {
        signals.Raise(signal);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && !is_matrix_signal_v<SignalT>, int>>
    void Reliquary::On(const std::function<void(const SignalT&)>& function)
    {
        signals.On(function);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && is_matrix_signal_v<SignalT>, int>>
    void Reliquary::On(const std::function<void(const SignalT&)>& function)
    {
        signals.On(function);
        matrices.EnsureInteraction<typename MatrixTypeForSignal<SignalT>::Type>(MatrixInteraction::Signal);
    }

    template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int>>
    Index<ShardT> Reliquary::CreateFromInternal(RelicID id, ConstructorArgs&& ... constructorArgs)
    {
        return shards.CreateFromInternal<ShardT>(id, std::forward<ConstructorArgs>(constructorArgs)...);
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
        Value value = nullptr;
    private:
        INSCRIPTION_ACCESS;
    };

    template<>
    class Scribe<KnownPolymorphic> final
    {
    public:
        using ObjectT = KnownPolymorphic;
    public:
        void Scriven(ObjectT& object, Format::Binary& format);
        void Scriven(const std::string& name, ObjectT& object, Format::Json& format);
    };

    template<>
    class Scribe<Arca::Reliquary> final
    {
    public:
        using ObjectT = Arca::Reliquary;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format);
    private:
        using KnownPolymorphicSerializerList = std::vector<Arca::KnownPolymorphicSerializer*>;
        template<class T>
        KnownPolymorphicSerializerList ToKnownPolymorphicSerializerList(std::vector<std::unique_ptr<T>>& stored);
    private:
        ReliquaryUserContext userContext;
    private:
        struct LoadedRelicMetadata
        {
            Arca::RelicID id = Arca::nullRelicID;

            std::vector<Arca::RelicID> children;
        };
        std::vector<LoadedRelicMetadata> loadedRelicMetadata;

        using MetadataExtension = std::tuple<Arca::Type, Arca::Locality, void*>;
        static MetadataExtension FindExtensionForLoadedMetadata(Arca::RelicID id, ObjectT& object);
    private:
        struct TypePair
        {
            Arca::TypeName arca;
            Type inscription;
        };
    private:
        static Arca::KnownPolymorphicSerializer* FindFrom(
            Type mainType,
            KnownPolymorphicSerializerList& list);
    private:
        void Save(ObjectT& object, Format::OutputBinary& format);
        void Load(ObjectT& object, Format::InputBinary& format);

        static void SaveAll(
            ObjectT& object,
            Format::OutputBinary& format,
            KnownPolymorphicSerializerList& polymorphicsFromObject);

        static void LoadAll(
            ObjectT& object,
            Format::InputBinary& format,
            KnownPolymorphicSerializerList& polymorphicsFromObject);

        static std::vector<TypePair> PruneTypesToLoad(
            KnownPolymorphicSerializerList& fromObject,
            Format::InputBinary& format,
            const std::vector<Type>& typesFromFormat);

        static std::vector<TypePair> ExtractTypes(
            KnownPolymorphicSerializerList& fromObject,
            Format::InputBinary& format);

        static void SaveRelicMetadata(Arca::RelicMetadata& metadata, Format::OutputBinary& format);
        static LoadedRelicMetadata LoadRelicMetadata(Format::InputBinary& format);
    private:
        void Save(ObjectT& object, Format::OutputJson& format);
        void Load(ObjectT& object, Format::InputJson& format);

        static void SaveAll(
            const std::string& name,
            ObjectT& object,
            Format::OutputJson& format,
            KnownPolymorphicSerializerList& polymorphicsFromObject);

        static void LoadAll(
            const std::string& name,
            ObjectT& object,
            Format::InputJson& format,
            KnownPolymorphicSerializerList& polymorphicsFromObject);

        static void SaveRelicMetadata(Arca::RelicMetadata& metadata, Format::OutputJson& format, const std::string& name);
        static LoadedRelicMetadata LoadRelicMetadata(Format::InputJson& format, const std::string& name);
        static void SetupLoadedRelicMetadata(
            const std::vector<LoadedRelicMetadata>& loadedRelicMetadata, ObjectT& object);

        static std::vector<TypePair> LoadTypes(
            KnownPolymorphicSerializerList& fromObject,
            Format::InputJson& format);

        static std::vector<TypePair> ExtractTypes(
            KnownPolymorphicSerializerList& fromObject,
            Format::InputJson& format);
    private:
        static void SignalCreation(ObjectT& object);
    };

    template<class Format>
    void Scribe<Arca::Reliquary>::Scriven(ObjectT& object, Format& format)
    {
        userContext.reliquary = &object;
        format.EmplaceUserContext(&userContext);

        if (format.IsInput())
            object.relics.Clear();
        
        format("nextRelicId", object.relics.nextRelicID);

        if (format.IsOutput())
            Save(object, *format.AsOutput());
        else
        {
            Load(object, *format.AsInput());
            SignalCreation(object);
        }

        format.template RemoveUserContext<ReliquaryUserContext>();
    }

    template<class T>
    auto Scribe<Arca::Reliquary>::ToKnownPolymorphicSerializerList(std::vector<std::unique_ptr<T>>& stored)
        -> KnownPolymorphicSerializerList
    {
        KnownPolymorphicSerializerList list;
        list.reserve(stored.size());
        for (auto& loop : stored)
            list.push_back(loop.get());
        return list;
    }

    template<class Format>
    struct ScribeTraits<Arca::Reliquary, Format> final
    {
        using Category = CompositeScribeCategory<Arca::Reliquary>;
    };
}

#include "RelicIndexDefinition.h"
#include "MatrixIndexDefinition.h"
#include "GlobalIndexDefinition.h"
#include "ShardIndexDefinition.h"

#include "ShardBatchSourceDefinition.h"