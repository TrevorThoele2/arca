#pragma once

#include "ReliquaryShards.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, class... ConstructorArgs>
    Index<ShardT> ReliquaryShards::Create(RelicID id, ConstructorArgs&& ... constructorArgs)
    {
        Relics().ShardModificationRequired(id);

        return CreateCommon<ShardT>(id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class ShardT, class... ConstructorArgs>
    Index<ShardT> ReliquaryShards::CreateFromInternal(RelicID id, ConstructorArgs&& ... constructorArgs)
    {
        return CreateCommon<ShardT>(id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::Destroy(RelicID id)
    {
        auto batchSource = FindBatchSource<ShardT>();
        if (batchSource != nullptr)
        {
            if (batchSource->ContainsFromBase(id))
            {
                Owner().Raise(DestroyingKnown<ShardT>{CreateIndex<ShardT>(id)});
                Owner().Raise(Destroying{ HandleFrom(id, batchSource->Type(), HandleObjectType::Shard) });

                batchSource->DestroyFromBase(id);
            }
        }
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::TransactionalDestroy(RelicID id)
    {
        auto batchSource = FindBatchSource<ShardT>();
        if (batchSource != nullptr)
        {
            auto matrixTransaction = Owner().matrices.StartDestroyingTransaction(id);

            if (batchSource->ContainsFromBase(id))
            {
                matrixTransaction.Finalize(TypeFor<ShardT>());

                Owner().Raise(DestroyingKnown<ShardT>{CreateIndex<ShardT>(id)});
                Owner().Raise(Destroying{ HandleFrom(id, batchSource->Type(), HandleObjectType::Shard) });

                batchSource->DestroyFromBase(id); 
            }
        }
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        auto& batchSource = RequiredBatchSource<ShardT>();
        auto found = batchSource.Find(id);
        return static_cast<bool>(found);
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
        return batchSource.Find(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::SignalCreation(const Index<ShardT>& index)
    {
        Owner().Raise(Created{ HandleFrom(index.ID(), TypeFor<ShardT>(), HandleObjectType::Shard) });
        Owner().Raise(CreatedKnown<ShardT>{index});
    }

    template<class ShardT>
    ReliquaryShards::Handler<ShardT>::Handler(Reliquary& reliquary) :
        HandlerBase(TypeFor<ShardT>().name),
        batchSource(reliquary), constBatchSource(reliquary)
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
    void ReliquaryShards::Handler<ShardT>::Create(RelicID id, Reliquary& reliquary, bool isConst)
    {
        CreateCommon(id, reliquary, isConst);
    }

    template<class ShardT>
    template<class... ConstructorArgs>
    void ReliquaryShards::Handler<ShardT>::CreateCommon(RelicID id, Reliquary& reliquary, bool isConst, ConstructorArgs&& ... constructorArgs)
    {
        auto matrixTransaction = reliquary.matrices.StartCreationTransaction(id);

        if (isConst)
            reliquary.shards
            .FindBatchSource<const ShardT>()
            ->Add(id, std::forward<ConstructorArgs>(constructorArgs)...);
        else
            reliquary.shards
            .FindBatchSource<ShardT>()
            ->Add(id, std::forward<ConstructorArgs>(constructorArgs)...);

        matrixTransaction.Finalize();
        reliquary.shards.SignalCreation(reliquary.shards.CreateIndex<ShardT>(id));
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::RequiredDestroy(RelicID id, Reliquary& reliquary)
    {
        reliquary.shards.Destroy<ShardT>(id);
        reliquary.shards.Destroy<const ShardT>(id);
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::Clear()
    {
        batchSource.Clear();
        constBatchSource.Clear();
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::SignalAllCreated(Reliquary& reliquary)
    {
        for (auto& shard : batchSource)
            reliquary.shards.SignalCreation(Index<ShardT>(shard.first, reliquary));

        for (auto& shard : constBatchSource)
            reliquary.shards.SignalCreation(Index<const ShardT>(shard.first, reliquary));
    }

    template<class ShardT>
    bool ReliquaryShards::Handler<ShardT>::Contains(RelicID id) const
    {
        return batchSource.ContainsFromBase(id) || constBatchSource.ContainsFromBase(id);
    }

    template<class ShardT>
    bool ReliquaryShards::Handler<ShardT>::WillBinarySerialize() const
    {
        return HasScribe<ShardT, Inscription::BinaryArchive>();
    }

    template<class ShardT>
    bool ReliquaryShards::Handler<ShardT>::WillJsonSerialize() const
    {
        return HasScribe<ShardT, Inscription::JsonArchive>();
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::Serialize(Inscription::BinaryArchive& archive)
    {
        archive(batchSource);
        archive(constBatchSource);
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::Serialize(const std::string& name, Inscription::JsonArchive& archive)
    {
        const auto constName = !name.empty() ? name + "Const" : "";

        archive(name, batchSource);
        archive(constName, constBatchSource);
    }

    template<class ShardT>
    std::vector<::Inscription::Type> ReliquaryShards::Handler<ShardT>::InscriptionTypes(Inscription::BinaryArchive& archive) const
    {
        return Inscription::InputTypesFor<std::remove_const_t<ShardT>>(archive);
    }

    template<class ShardT>
    std::vector<::Inscription::Type> ReliquaryShards::Handler<ShardT>::InscriptionTypes(Inscription::JsonArchive& archive) const
    {
        return Inscription::InputTypesFor<std::remove_const_t<ShardT>>(archive);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::CreateHandler()
    {
        handlers.push_back(std::make_unique<Handler<ShardT>>(Owner()));
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ReliquaryShards::Handler<ShardT>* ReliquaryShards::FindHandler() const
    {
        auto found = FindHandler(TypeFor<ShardT>().name);
        if (found == nullptr)
            return nullptr;

        return static_cast<Handler<ShardT>*>(found);
    }

    template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int>>
    BatchSource<ObjectT>* ReliquaryShards::FindBatchSource() const
    {
        const auto type = TypeFor<ObjectT>();
        auto batchSource = FindBatchSource(type);
        if (batchSource == nullptr)
            return nullptr;

        return static_cast<BatchSource<ObjectT>*>(batchSource);
    }

    template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int>>
    BatchSource<ObjectT>& ReliquaryShards::RequiredBatchSource() const
    {
        auto found = FindBatchSource<ObjectT>();
        if (!found)
        {
            const auto type = TypeFor<ObjectT>();
            throw NotRegistered(type, typeid(ObjectT));
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
    Index<ShardT> ReliquaryShards::CreateCommon(RelicID id, ConstructorArgs&& ... constructorArgs)
    {
        const auto type = TypeFor<ShardT>();
        if (Contains(Handle(id, Owner(), { type.name, true }, HandleObjectType::Shard)) ||
            Contains(Handle(id, Owner(), { type.name, false }, HandleObjectType::Shard)))
            throw CannotCreate(type);

        auto handler = FindHandler<ShardT>();
        if (!handler)
            throw NotRegistered(type, typeid(ShardT));
        handler->CreateCommon(id, Owner(), std::is_const_v<ShardT>, std::forward<ConstructorArgs>(constructorArgs)...);

        return CreateIndex<ShardT>(id);
    }

    template<class T>
    auto ReliquaryShards::CreateIndex(RelicID id) const
    {
        return ToReference<T>(id, const_cast<Reliquary&>(Owner()));
    }
}