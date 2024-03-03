#pragma once

#include <unordered_map>
#include <optional>
#include <any>

#include "ReliquaryException.h"

#include "DynamicRelic.h"
#include "TypedRelic.h"
#include "RelicStructure.h"
#include "RelicTraits.h"
#include "HasFactoryMethod.h"
#include "RelicMetadata.h"
#include "RelicCreated.h"
#include "BeforeRelicDestroyed.h"

#include "RelicBatch.h"
#include "RelicBatchSource.h"
#include "ChildRelicBatch.h"
#include "ChildRelicBatchSource.h"

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
        DynamicRelic CreateRelic();
        DynamicRelic CreateRelic(const RelicStructure& structure);
        DynamicRelic CreateRelic(const std::string& structureName);
        template<class RelicT, class... CreationArgs, std::enable_if_t<has_factory_method_v<RelicT>, int> = 0>
        RelicT* CreateRelic(CreationArgs&& ... creationArgs);
        template<class RelicT, class... CreationArgs, std::enable_if_t<!has_factory_method_v<RelicT>, int> = 0>
        RelicT* CreateRelic(CreationArgs&& ... creationArgs);

        void ParentRelic(RelicID parent, RelicID child);

        void DestroyRelic(RelicID id);
        void DestroyRelic(DynamicRelic& relic);
        template<class RelicT>
        void DestroyRelic(RelicT& relic);

        std::optional<DynamicRelic> FindRelic(RelicID id);
        template<class RelicT>
        RelicT* FindRelic(RelicID id);
        template<class RelicT>
        [[nodiscard]] RelicT* StaticRelic();

        template<class RelicT>
        [[nodiscard]] RelicBatch<RelicT> RelicBatch();
        template<class RelicT>
        [[nodiscard]] ChildRelicBatch<RelicT> ChildRelicBatch(RelicID parentID);

        [[nodiscard]] SizeT RelicCount() const;
    public:
        template<class ShardT>
        [[nodiscard]] ShardT* FindShard(RelicID id);

        template<class ShardT>
        [[nodiscard]] ShardBatch<ShardT> ShardBatch();
    public:
        Curator* FindCurator(const TypeHandle& typeHandle);
        template<class CuratorT>
        [[nodiscard]] CuratorT* FindCurator();
        template<class CuratorT>
        [[nodiscard]] const CuratorT* FindCurator() const;
    public:
        template<class SignalT>
        void RaiseSignal(const SignalT& signal);

        template<class SignalT>
        void ExecuteOnSignal(const std::function<void(const SignalT&)>& function);

        template<class SignalT>
        SignalBatch<SignalT> SignalBatch();
    private:
        struct KnownPolymorphicSerializer
        {
            using Serializer = std::function<void(void*, Reliquary&, ::Inscription::BinaryArchive&)>;
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

        template<class RelicT>
        void SignalRelicCreation(RelicT& relic);
        template<class RelicT>
        void SignalRelicDestruction(RelicT& relic);
        void SignalRelicParented(RelicMetadata parent, RelicMetadata child);

        template<class RelicT>
        static TypeHandle TypeHandleForRelic();
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

        KnownPolymorphicSerializerMap relicSerializerMap;

        [[nodiscard]] RelicBatchSourceBase* FindRelicBatchSource(const TypeHandle& typeHandle);
        template<class RelicT>
        [[nodiscard]] RelicBatchSource<RelicT>* FindRelicBatchSource();
        template<class RelicT>
        [[nodiscard]] RelicBatchSource<RelicT>& RequiredRelicBatchSource();
    private:
        using ChildRelicBatchSourceList = std::list<std::unique_ptr<ChildRelicBatchSourceBase>>;
        using ChildRelicBatchSourceMap = std::unordered_map<TypeHandle, ChildRelicBatchSourceList>;
        ChildRelicBatchSourceMap childRelicBatchSources;

        void NotifyChildRelicBatchSourcesAdd(RelicID parentID, void* childStorage, const TypeHandle& childTypeHandle);
        void NotifyChildRelicBatchSourcesDestroy(RelicID parentID, RelicID childID, const TypeHandle& childTypeHandle);

        [[nodiscard]] ChildRelicBatchSourceBase* FindChildRelicBatchSource(const TypeHandle& typeHandle, RelicID parentID);
        [[nodiscard]] ChildRelicBatchSourceList* FindChildRelicBatchSourceList(const TypeHandle& typeHandle);
        [[nodiscard]] ChildRelicBatchSourceList& RequiredChildRelicBatchSourceList(const TypeHandle& typeHandle);
    private:
        using StaticRelicMap = std::unordered_map<TypeHandle, std::any>;
        StaticRelicMap staticRelicMap;

        KnownPolymorphicSerializerMap staticRelicSerializerMap;
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
        void SignalShardCreation(ShardT& shard);
        template<class ShardT>
        void SignalShardDestruction(ShardT& shard);

        template<class ShardT>
        ShardFactory FindShardFactory();

        template<class ShardT>
        static TypeHandle TypeHandleForShard();
    private:
        using ShardBatchSourcePtr = std::unique_ptr<ShardBatchSourceBase>;
        using ShardBatchSourceMap = std::unordered_map<TypeHandle, ShardBatchSourcePtr>;
        ShardBatchSourceMap shardBatchSources;

        KnownPolymorphicSerializerMap shardSerializerMap;

        [[nodiscard]] ShardBatchSourceBase* FindShardBatchSource(const TypeHandle& typeHandle);
        template<class ShardT>
        [[nodiscard]] ShardBatchSource<ShardT>* FindShardBatchSource();
        template<class ShardT>
        [[nodiscard]] ShardBatchSource<ShardT>& RequiredShardBatchSource();
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
        template<class SignalT>
        SignalBatchSource<SignalT>* FindSignalBatchSource();
        template<class SignalT>
        const SignalBatchSource<SignalT>* FindSignalBatchSource() const;

        template<class SignalT>
        static std::type_index KeyForSignalBatchSource();
    private:
        friend class ReliquaryOrigin;
        friend class DynamicRelic;
        template<class... Shards>
        friend class TypedRelicWithShards;
        friend class TypeRegistration;
        template<class T>
        friend class ShardBatchSource;
        template<class T>
        friend class SignalBatchSource;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class RelicT, class... CreationArgs, std::enable_if_t<has_factory_method_v<RelicT>, int>>
    RelicT* Reliquary::CreateRelic(CreationArgs&& ... creationArgs)
    {
        const auto id = NextRelicID();
        std::optional<RelicT> relic = RelicTraits<RelicT>::Factory(*this, std::forward<CreationArgs>(creationArgs)...);
        if (!relic)
            return nullptr;
        relic->id = id;

        auto& batchSource = RequiredRelicBatchSource<RelicT>();
        auto added = batchSource.Add(*relic);

        SetupNewRelicInternals(id, RelicDynamism::Fixed, RelicTraits<RelicT>::typeHandle, added);
        SatisfyRelicStructure(relic->Structure(), id);
        SignalRelicCreation(*added);

        added->Initialize(*this);

        return added;
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<!has_factory_method_v<RelicT>, int>>
    RelicT* Reliquary::CreateRelic(CreationArgs&& ... creationArgs)
    {
        const auto id = NextRelicID();
        RelicT relic(std::forward<CreationArgs>(creationArgs)...);
        relic.id = id;

        auto& batchSource = RequiredRelicBatchSource<RelicT>();
        auto added = batchSource.Add(relic);

        SetupNewRelicInternals(id, RelicDynamism::Fixed, RelicTraits<RelicT>::typeHandle, added);
        SatisfyRelicStructure(relic.Structure(), id);
        SignalRelicCreation(*added);

        added->Initialize(*this);

        return added;
    }

    template<class RelicT>
    void Reliquary::DestroyRelic(RelicT& relic)
    {
        const auto metadata = RelicMetadataFor(relic.ID());
        if (!WillDestroyRelic(metadata))
            return;

        SignalRelicDestruction(relic);
        DestroyRelic(*metadata);
    }

    template<class RelicT>
    RelicT* Reliquary::FindRelic(RelicID id)
    {
        const auto metadata = RelicMetadataFor(id);
        if (!metadata)
            return {};

        auto& batchSource = RequiredRelicBatchSource<RelicT>();
        return batchSource.Find(id);
    }

    template<class RelicT>
    RelicT* Reliquary::StaticRelic()
    {
        const auto typeHandle = RelicTraits<RelicT>::typeHandle;
        const auto found = staticRelicMap.find(typeHandle);
        if (found == staticRelicMap.end())
            throw NotRegistered("static relic", typeHandle);

        return std::any_cast<RelicT>(&found->second);
    }

    template<class RelicT>
    RelicBatch<RelicT> Reliquary::RelicBatch()
    {
        const auto typeHandle = TypeHandleForRelic<RelicT>();
        auto& batchSource = RequiredRelicBatchSource<RelicT>();
        return Arca::RelicBatch<RelicT>(batchSource);
    }

    template<class RelicT>
    ChildRelicBatch<RelicT> Reliquary::ChildRelicBatch(RelicID parentID)
    {
        using SourceT = ChildRelicBatchSource<RelicT>;

        auto& _ = RequiredRelicBatchSource<RelicT>();

        const auto typeHandle = TypeHandleForRelic<RelicT>();
        auto& batchSourceList = RequiredChildRelicBatchSourceList(typeHandle);
        for (auto& loop : batchSourceList)
            if (loop->Parent() == parentID)
                return Arca::ChildRelicBatch<RelicT>(*static_cast<SourceT*>(loop.get()));
        batchSourceList.push_back(std::make_unique<SourceT>(parentID));
        auto& batchSource = *static_cast<SourceT*>(batchSourceList.back().get());
        for(auto& metadata : relicMetadataList)
            if (batchSource.Parent() == metadata.parent)
                batchSource.Add(reinterpret_cast<RelicT*>(metadata.storage));
        return Arca::ChildRelicBatch<RelicT>(batchSource);
    }

    template<class ShardT>
    ShardT* Reliquary::FindShard(RelicID id)
    {
        auto& batch = RequiredShardBatchSource<ShardT>();
        return batch.Find(id);
    }

    template<class ShardT>
    ShardBatch<ShardT> Reliquary::ShardBatch()
    {
        const auto typeHandle = TypeHandleForShard<ShardT>();
        auto batchSource = FindShardBatchSource<ShardT>();
        if (!batchSource)
            throw NotRegistered("shard", typeHandle);

        return Arca::ShardBatch<ShardT>(*batchSource);
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
        ExecuteAllForSignal(signal);
    }

    template<class SignalT>
    void Reliquary::ExecuteOnSignal(const std::function<void(const SignalT&)>& function)
    {
        const auto typeIndex = std::type_index(typeid(SignalT));
        auto found = signalExecutionMap.find(typeIndex);
        if (found == signalExecutionMap.end())
            found = signalExecutionMap.emplace(typeIndex, SignalExecutionList<SignalT>()).first;

        auto& executionList = std::any_cast<SignalExecutionList<SignalT>&>(found->second);
        executionList.push_back(function);
    }

    template<class SignalT>
    SignalBatch<SignalT> Reliquary::SignalBatch()
    {
        auto signalBatchSource = FindSignalBatchSource<SignalT>();
        if (!signalBatchSource)
            throw NotRegistered("signal", typeid(SignalT).name());

        return Arca::SignalBatch<SignalT>(*signalBatchSource);
    }

    template<class RelicT>
    void Reliquary::SignalRelicCreation(RelicT& relic)
    {
        RelicCreated<RelicT> signal{ relic };
        RaiseSignal(signal);
    }

    template<class RelicT>
    void Reliquary::SignalRelicDestruction(RelicT& relic)
    {
        BeforeRelicDestroyed<RelicT> signal{ relic };
        RaiseSignal(signal);
    }

    template<class RelicT>
    TypeHandle Reliquary::TypeHandleForRelic()
    {
        return RelicTraits<RelicT>::typeHandle;
    }

    template<class RelicT>
    RelicBatchSource<RelicT>* Reliquary::FindRelicBatchSource()
    {
        auto found = relicBatchSources.find(TypeHandleForRelic<RelicT>());
        if (found == relicBatchSources.end())
            return nullptr;

        return static_cast<RelicBatchSource<RelicT>*>(found->second.get());
    }

    template<class RelicT>
    RelicBatchSource<RelicT>& Reliquary::RequiredRelicBatchSource()
    {
        const auto typeHandle = TypeHandleForRelic<RelicT>();
        auto found = FindRelicBatchSource<RelicT>();
        if (!found)
            throw NotRegistered("relic", typeHandle);

        return *found;
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
        SignalShardCreation(*added);
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
    void Reliquary::SignalShardCreation(ShardT& shard)
    {
        ShardCreated<ShardT> signal{ shard };
        RaiseSignal(signal);
    }

    template<class ShardT>
    void Reliquary::SignalShardDestruction(ShardT& shard)
    {
        BeforeShardDestroyed<ShardT> signal{ shard };
        RaiseSignal(signal);
    }

    template<class ShardT>
    auto Reliquary::FindShardFactory() -> ShardFactory
    {
        auto typeHandle = TypeHandleForShard<ShardT>();
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

    template<class ShardT>
    ShardBatchSource<ShardT>* Reliquary::FindShardBatchSource()
    {
        auto found = shardBatchSources.find(TypeHandleForShard<ShardT>());
        if (found == shardBatchSources.end())
            return nullptr;

        return static_cast<ShardBatchSource<ShardT>*>(found->second.get());
    }

    template<class ShardT>
    ShardBatchSource<ShardT>& Reliquary::RequiredShardBatchSource()
    {
        const auto typeHandle = TypeHandleForShard<ShardT>();
        auto found = FindShardBatchSource<ShardT>();
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
            knownPolymorphics.emplace_back(piece, object, *serializer);
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
            auto adapter = KnownPolymorphic(serializedObject, object, *serializer);
            jumpTable.FillObject(typeToLoad.inscription, adapter, archive);
        }
    }
}

#include "DynamicRelicDefinition.h"