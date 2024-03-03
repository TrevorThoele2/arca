#pragma once

#include "ShardBatch.h"

#include "ReliquarySignals.h"
#include "ReliquaryMatrices.h"
#include "ReliquaryException.h"

#include "IsShard.h"
#include "Index.h"
#include "ShardIndex.h"
#include "Handle.h"

#include "Created.h"
#include "Destroying.h"

#include "RelicID.h"
#include "Type.h"
#include <Chroma/VariadicTemplate.h>
#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class Reliquary;

    class ReliquaryShards
    {
    public:
        void Create(const Type& type, RelicID id, bool required);
        template<class ShardT, class... ConstructorArgs>
        Index<ShardT> Create(RelicID id, ConstructorArgs&& ... constructorArgs);
        template<class ShardT, class... ConstructorArgs>
        Index<ShardT> CreateFromInternal(RelicID id, ConstructorArgs&& ... constructorArgs);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy(RelicID id);
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        void Destroy(RelicID id);
        void TransactionalDestroy(const Type& type, RelicID id);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void TransactionalDestroy(RelicID id);
        void Clear();
        void Clear(RelicID id);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Index<ShardT> Find(RelicID id) const;

        [[nodiscard]] bool Contains(const Handle& handle) const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const ShardT& shard) const;

        [[nodiscard]] bool IsShardTypeName(const TypeName& typeName) const;
        [[nodiscard]] std::vector<TypeName> AllTypeNames() const;
        [[nodiscard]] std::unordered_set<Type> AllTypes(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void SignalCreation(const Index<ShardT>& index);

        [[nodiscard]] size_t Size(const Type& type) const;

        void AddType(RelicID id, const Type& type);
        void RemoveType(RelicID id, const Type& type);
    public:
        class HandlerBase : public KnownPolymorphicSerializer
        {
        public:
            const Type type;
        public:
            virtual ~HandlerBase() = 0;

            virtual ShardBatchSourceBase& BatchSource() = 0;
            virtual ShardBatchSourceBase& ConstBatchSource() = 0;

            virtual void Create(
                RelicID id,
                ReliquaryMatrices& matrices,
                Reliquary& reliquary,
                bool isConst,
                bool required) = 0;
            virtual void RequiredDestroy(
                RelicID id,
                Reliquary& reliquary,
                ReliquarySignals& signals) = 0;
            virtual void RequiredTransactionalDestroy(
                RelicID id,
                Reliquary& reliquary,
                ReliquarySignals& signals,
                ReliquaryMatrices& matrices) = 0;
            virtual void Clear() = 0;

            virtual void SignalAllCreated(Reliquary& reliquary, ReliquaryShards& shards) = 0;

            [[nodiscard]] virtual bool Contains(RelicID id) const = 0;
            [[nodiscard]] virtual bool IsRequired(RelicID id) const = 0;

            [[nodiscard]] TypeName MainType() const override;
        protected:
            explicit HandlerBase(const Type& type);
        };

        template<class ShardT>
        class Handler final : public HandlerBase
        {
        public:
            Arca::BatchSource<ShardT> batchSource;
            Arca::BatchSource<const ShardT> constBatchSource;
        public:
            explicit Handler(Reliquary& reliquary, ReliquaryShards& shards, ReliquaryMatrices& matrices);

            ShardBatchSourceBase& BatchSource() override;
            ShardBatchSourceBase& ConstBatchSource() override;

            void Create(
                RelicID id,
                ReliquaryMatrices& matrices,
                Reliquary& reliquary,
                bool isConst,
                bool required) override;
            template<class... ConstructorArgs>
            void CreateCommon(
                RelicID id,
                ReliquaryMatrices& matrices,
                Reliquary& reliquary,
                bool isConst,
                bool required,
                ConstructorArgs&& ... constructorArgs);
            void RequiredDestroy(
                RelicID id,
                Reliquary& reliquary,
                ReliquarySignals& signals) override;
            void RequiredTransactionalDestroy(
                RelicID id,
                Reliquary& reliquary,
                ReliquarySignals& signals,
                ReliquaryMatrices& matrices) override;
            void Clear() override;

            void SignalAllCreated(Reliquary& reliquary, ReliquaryShards& shards) override;

            [[nodiscard]] bool Contains(RelicID id) const override;
            [[nodiscard]] bool IsRequired(RelicID id) const override;

            [[nodiscard]] bool WillBinarySerialize() const override;
            [[nodiscard]] bool WillJsonSerialize() const override;
            void Serialize(Inscription::Archive::Binary& archive) override;
            void Serialize(const std::string& name, Inscription::Archive::Json& archive) override;
            [[nodiscard]] std::vector<Inscription::Type> InscriptionTypes(Inscription::Archive::Binary& archive) const override;
            [[nodiscard]] std::vector<Inscription::Type> InscriptionTypes(Inscription::Archive::Json& archive) const override;
        private:
            template<class BatchSourceT>
            void DestroyCommon(
                RelicID id,
                Reliquary& reliquary,
                ReliquarySignals& signals,
                BatchSourceT& batchSource);
            template<class BatchSourceT>
            void TransactionalDestroyCommon(
                RelicID id,
                Reliquary& reliquary,
                ReliquarySignals& signals,
                ReliquaryMatrices& matrices,
                BatchSourceT& batchSource);

            void AddAllTypes();
            template<class BatchSourceT>
            [[nodiscard]] std::vector<RelicID> AllIDs(BatchSourceT& batchSource) const;
        private:
            ReliquaryShards* shards;
        };

        using HandlerPtr = std::unique_ptr<HandlerBase>;
        using HandlerList = std::vector<HandlerPtr>;
        HandlerList handlers;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void CreateHandler();
        [[nodiscard]] HandlerBase* FindHandler(const TypeName& typeName) const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Handler<ShardT>* FindHandler() const;

        [[nodiscard]] ShardBatchSourceBase* FindBatchSource(const Type& type) const;
        template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int> = 0>
        [[nodiscard]] BatchSource<ObjectT>* FindBatchSource() const;

        [[nodiscard]] ShardBatchSourceBase& RequiredBatchSource(const Type& type) const;
        template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int> = 0>
        [[nodiscard]] BatchSource<ObjectT>& RequiredBatchSource() const;

        template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int> = 0>
        [[nodiscard]] Arca::Batch<ObjectT> Batch() const;
    public:
        ReliquaryShards(const ReliquaryShards& arg) = delete;
        ReliquaryShards& operator=(const ReliquaryShards& arg) = delete;
    private:
        template<class ShardT, class... ConstructorArgs>
        Index<ShardT> CreateCommon(RelicID id, bool required, ConstructorArgs&& ... constructorArgs);
    private:
        std::unordered_map<RelicID, std::unordered_set<Type>> idToTypes;
    private:
        template<Chroma::VariadicTemplateSize i>
        struct DestroyAllShardsIterator
        {
            template<class ShardPack>
            static void Do(ShardPack, RelicID id, ReliquaryShards& shards)
            {
                using T = typename ShardPack::template Parameter<i>::Type;

                shards.TransactionalDestroy<T>(id);
            }
        };

        template<Chroma::VariadicTemplateSize i>
        struct ContainsAllShardsIterator
        {
            template<class ShardPack>
            static bool Check(ShardPack, RelicID id, const ReliquaryShards& shards)
            {
                using T = typename ShardPack::template Parameter<i>::Type;

                return shards.Contains<T>(id);
            }
        };
    private:
        Reliquary* owner;
        ReliquarySignals* signals;
        ReliquaryMatrices* matrices;

        const std::string objectTypeName = "shard";
    private:
        explicit ReliquaryShards(Reliquary& owner, ReliquarySignals& signals, ReliquaryMatrices& matrices);
        ReliquaryShards(ReliquaryShards&& arg) noexcept = default;
        friend Reliquary;
    };

    template<class ShardT, class... ConstructorArgs>
    Index<ShardT> ReliquaryShards::Create(RelicID id, ConstructorArgs&& ... constructorArgs)
    {
        return CreateCommon<ShardT>(id, false, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class ShardT, class... ConstructorArgs>
    Index<ShardT> ReliquaryShards::CreateFromInternal(RelicID id, ConstructorArgs&& ... constructorArgs)
    {
        return CreateCommon<ShardT>(id, true, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::Destroy(RelicID id)
    {
        auto handler = FindHandler<ShardT>();
        if (handler)
        {
            if (handler->IsRequired(id))
                throw CannotDestroy(objectTypeName, TypeFor<ShardT>());

            handler->RequiredDestroy(id, *owner);
        }
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void ReliquaryShards::Destroy(RelicID id)
    {
        const auto matrixTypes = MatrixImplementation<MatrixT>::Types();
        for (auto& matrixType : matrixTypes)
            TransactionalDestroy(matrixType, id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::TransactionalDestroy(RelicID id)
    {
        auto handler = FindHandler<ShardT>();
        if (handler)
        {
            if (handler->IsRequired(id))
                throw CannotDestroy(objectTypeName, TypeFor<ShardT>());

            handler->RequiredTransactionalDestroy(id, *owner, *signals, *matrices);
        }
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Index<ShardT> ReliquaryShards::Find(RelicID id) const
    {
        auto batchSource = FindBatchSource<ShardT>();
        return batchSource ? Index<ShardT>{ id, *owner, batchSource->Find(id) } : Index<ShardT>{};
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        auto& batchSource = RequiredBatchSource<ShardT>();
        auto found = batchSource.Find(id);
        return static_cast<bool>(found.lock());
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        return MatrixImplementation<MatrixT>::IsIn(AllTypes(id));
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    RelicID ReliquaryShards::IDFor(const ShardT& shard) const
    {
        auto& batchSource = RequiredBatchSource<ShardT>();
        for (auto loop = batchSource.begin(); loop != batchSource.end(); ++loop)
            if (&loop->shard == &shard)
                return loop->id;

        return nullRelicID;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* ReliquaryShards::FindStorage(RelicID id)
    {
        auto& batchSource = RequiredBatchSource<ShardT>();
        return batchSource.Find(id).lock().get();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::SignalCreation(const Index<ShardT>& index)
    {
        signals->Raise(Created{ Handle{ index.ID(), TypeFor<ShardT>() } });
        signals->Raise(CreatedKnown<ShardT>{index});
    }

    template<class ShardT>
    ReliquaryShards::Handler<ShardT>::Handler(Reliquary& reliquary, ReliquaryShards& shards, ReliquaryMatrices& matrices) :
        HandlerBase(TypeFor<ShardT>()),
        batchSource(reliquary, shards, matrices),
        constBatchSource(reliquary, shards, matrices),
        shards(&shards)
    {}

    template<class ShardT>
    ShardBatchSourceBase& ReliquaryShards::Handler<ShardT>::BatchSource()
    {
        return batchSource;
    }

    template<class ShardT>
    ShardBatchSourceBase& ReliquaryShards::Handler<ShardT>::ConstBatchSource()
    {
        return constBatchSource;
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::Create(
        RelicID id, ReliquaryMatrices& matrices, Reliquary& reliquary, bool isConst, bool required)
    {
        CreateCommon(id, matrices, reliquary, isConst, required);
    }

    template<class ShardT>
    template<class... ConstructorArgs>
    void ReliquaryShards::Handler<ShardT>::CreateCommon(
        RelicID id,
        ReliquaryMatrices& matrices,
        Reliquary& reliquary,
        bool isConst,
        bool required,
        ConstructorArgs&& ... constructorArgs)
    {
        if (isConst)
        {
            const auto added = shards->FindBatchSource<const ShardT>()->Add(
                id, required, std::forward<ConstructorArgs>(constructorArgs)...);
            const auto type = TypeFor<ShardT>();
            shards->AddType(id, type);
            shards->SignalCreation(Index<const ShardT>{ id, reliquary, added });
            matrices.ShardCreated(id, type, shards->AllTypes(id));
        }
        else
        {
            const auto added = shards->FindBatchSource<ShardT>()->Add(
                id, required, std::forward<ConstructorArgs>(constructorArgs)...);
            const auto type = TypeFor<ShardT>();
            shards->AddType(id, type);
            shards->SignalCreation(Index<ShardT>{ id, reliquary, added });
            matrices.ShardCreated(id, type, shards->AllTypes(id));
        }
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::RequiredDestroy(
        RelicID id,
        Reliquary& reliquary,
        ReliquarySignals& signals)
    {
        DestroyCommon(id, reliquary, signals, batchSource);
        DestroyCommon(id, reliquary, signals, constBatchSource);
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::RequiredTransactionalDestroy(
        RelicID id,
        Reliquary& reliquary,
        ReliquarySignals& signals,
        ReliquaryMatrices& matrices)
    {
        TransactionalDestroyCommon(id, reliquary, signals, matrices, batchSource);
        TransactionalDestroyCommon(id, reliquary, signals, matrices, constBatchSource);
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::Clear()
    {
        batchSource.Clear();
        constBatchSource.Clear();
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::SignalAllCreated(Reliquary& reliquary, ReliquaryShards& shards)
    {
        for (auto& shard : batchSource)
            shards.SignalCreation(shards.Find<ShardT>(shard.first));

        for (auto& shard : constBatchSource)
            shards.SignalCreation(shards.Find<const ShardT>(shard.first));
    }

    template<class ShardT>
    bool ReliquaryShards::Handler<ShardT>::Contains(RelicID id) const
    {
        return batchSource.ContainsFromBase(id) || constBatchSource.ContainsFromBase(id);
    }

    template<class ShardT>
    bool ReliquaryShards::Handler<ShardT>::IsRequired(RelicID id) const
    {
        return batchSource.IsRequired(id) || constBatchSource.IsRequired(id);
    }

    template<class ShardT>
    bool ReliquaryShards::Handler<ShardT>::WillBinarySerialize() const
    {
        return HasScribe<ShardT, Inscription::Archive::Binary>();
    }

    template<class ShardT>
    bool ReliquaryShards::Handler<ShardT>::WillJsonSerialize() const
    {
        return HasScribe<ShardT, Inscription::Archive::Json>();
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::Serialize(Inscription::Archive::Binary& archive)
    {
        archive(batchSource);
        archive(constBatchSource);
        if (archive.IsInput())
            AddAllTypes();
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::Serialize(const std::string& name, Inscription::Archive::Json& archive)
    {
        const auto constName = !name.empty() ? name + "Const" : "";

        archive(name, batchSource);
        archive(constName, constBatchSource);
        if (archive.IsInput())
            AddAllTypes();
    }

    template<class ShardT>
    std::vector<Inscription::Type> ReliquaryShards::Handler<ShardT>::InscriptionTypes(Inscription::Archive::Binary& archive) const
    {
        return Inscription::InputTypesFor<std::remove_const_t<ShardT>>(archive);
    }

    template<class ShardT>
    std::vector<Inscription::Type> ReliquaryShards::Handler<ShardT>::InscriptionTypes(Inscription::Archive::Json& archive) const
    {
        return Inscription::InputTypesFor<std::remove_const_t<ShardT>>(archive);
    }

    template<class ShardT>
    template<class BatchSourceT>
    void ReliquaryShards::Handler<ShardT>::DestroyCommon(
        RelicID id,
        Reliquary& reliquary,
        ReliquarySignals& signals,
        BatchSourceT& batchSource)
    {
        using RealShardT = typename BatchSourceT::ShardT;

        if (batchSource.ContainsFromBase(id))
        {
            signals.Raise(DestroyingKnown<RealShardT>{ Index<RealShardT>{id, reliquary, batchSource.Find(id)} });
            signals.Raise(Destroying{ Handle{ id, std::move(batchSource.Type()) } });

            batchSource.DestroyFromBase(id);

            shards->RemoveType(id, TypeFor<ShardT>());
        }
    }

    template<class ShardT>
    template<class BatchSourceT>
    void ReliquaryShards::Handler<ShardT>::TransactionalDestroyCommon(
        RelicID id,
        Reliquary& reliquary,
        ReliquarySignals& signals,
        ReliquaryMatrices& matrices, 
        BatchSourceT& batchSource)
    {
        using RealShardT = typename BatchSourceT::ShardT;

        if (batchSource.ContainsFromBase(id))
        {
            matrices.ShardDestroying(id, TypeFor<RealShardT>(), shards->AllTypes(id));

            signals.Raise(DestroyingKnown<RealShardT>{Index<RealShardT>{id, reliquary, batchSource.Find(id)}});
            signals.Raise(Destroying{ Handle{ id, std::move(batchSource.Type()) } });

            batchSource.DestroyFromBase(id);

            shards->RemoveType(id, TypeFor<ShardT>());
        }
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::AddAllTypes()
    {
        for (auto id : AllIDs(batchSource))
            shards->AddType(id, TypeFor<ShardT>());
        for (auto id : AllIDs(constBatchSource))
            shards->AddType(id, TypeFor<const ShardT>());
    }

    template<class ShardT>
    template<class BatchSourceT>
    std::vector<RelicID> ReliquaryShards::Handler<ShardT>::AllIDs(BatchSourceT& batchSource) const
    {
        std::vector<RelicID> ids;
        for (auto& shard : batchSource)
            ids.push_back(shard.first);
        return ids;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::CreateHandler()
    {
        handlers.push_back(std::make_unique<Handler<ShardT>>(*owner, *this, *matrices));
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ReliquaryShards::Handler<ShardT>* ReliquaryShards::FindHandler() const
    {
        auto found = FindHandler(TypeFor<ShardT>().name);
        return found == nullptr ? nullptr : static_cast<Handler<ShardT>*>(found);
    }

    template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int>>
    BatchSource<ObjectT>* ReliquaryShards::FindBatchSource() const
    {
        const auto type = TypeFor<ObjectT>();
        auto batchSource = FindBatchSource(type);
        return batchSource == nullptr ? nullptr : static_cast<BatchSource<ObjectT>*>(batchSource);
    }

    template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int>>
    BatchSource<ObjectT>& ReliquaryShards::RequiredBatchSource() const
    {
        auto found = FindBatchSource<ObjectT>();
        if (!found)
        {
            const auto type = TypeFor<ObjectT>();
            throw NotRegistered(objectTypeName, type, typeid(ObjectT));
        }

        return *found;
    }

    template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int>>
    Arca::Batch<ObjectT> ReliquaryShards::Batch() const
    {
        auto& batchSource = RequiredBatchSource<ObjectT>();
        return Arca::Batch<ObjectT>(batchSource);
    }

    template<class ShardT, class... ConstructorArgs>
    Index<ShardT> ReliquaryShards::CreateCommon(RelicID id, bool required, ConstructorArgs&& ... constructorArgs)
    {
        const auto type = TypeFor<ShardT>();
        if (Contains(Handle{ id, { type.name, true } }) || Contains(Handle{ id, { type.name, false } }))
            throw CannotCreate(objectTypeName, type);

        auto handler = FindHandler<ShardT>();
        if (!handler)
            throw NotRegistered(objectTypeName, type, typeid(ShardT));
        handler->CreateCommon(
            id,
            *matrices,
            *owner,
            std::is_const_v<ShardT>,
            required,
            std::forward<ConstructorArgs>(constructorArgs)...);

        return Find<ShardT>(id);
    }
}