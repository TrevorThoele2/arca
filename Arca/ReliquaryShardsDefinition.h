#pragma once

#include "ReliquaryShards.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT>
    ShardIndex<ShardT> ReliquaryShards::Create(RelicID id)
    {
        Relics().ShardModificationRequired(id);

        return CreateCommon<ShardT>(id);
    }

    template<class ShardT>
    ShardIndex<ShardT> ReliquaryShards::CreateFromInternal(RelicID id)
    {
        return CreateCommon<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::Destroy(RelicID id)
    {
        Relics().ShardModificationRequired(id);

        auto batchSource = FindBatchSource<ShardT>();
        if (batchSource != nullptr)
        {
            if (batchSource->DestroyFromBase(id))
            {
                Owner().matrices.NotifyDestroying(id);
                Owner().Raise<DestroyingKnown<ShardT>>(CreateIndex<ShardT>(id));
                Owner().Raise<Destroying>(HandleFrom(id, batchSource->Type(), HandleObjectType::Shard));
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

        return 0;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* ReliquaryShards::FindStorage(RelicID id)
    {
        auto& batchSource = RequiredBatchSource<ShardT>();
        return batchSource.Find(id);
    }

    template<class ShardT>
    ReliquaryShards::Handler<ShardT>::Handler() : HandlerBase(TypeFor<ShardT>().name)
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
        if (isConst)
            reliquary.shards.FindBatchSource<const ShardT>()->Add(id);
        else
            reliquary.shards.FindBatchSource<ShardT>()->Add(id);

        reliquary.matrices.NotifyCreated(id);
        reliquary.Raise<Created>(reliquary.shards.HandleFrom(id, TypeFor<ShardT>(), HandleObjectType::Shard));
        reliquary.Raise<CreatedKnown<ShardT>>(reliquary.shards.CreateIndex<ShardT>(id));
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::Destroy(RelicID id, Reliquary& reliquary)
    {
        reliquary.shards.Destroy<ShardT>(id);
    }

    template<class ShardT>
    bool ReliquaryShards::Handler<ShardT>::WillSerialize() const
    {
        return HasScribe<ShardT>();
    }

    template<class ShardT>
    void ReliquaryShards::Handler<ShardT>::Serialize(Inscription::BinaryArchive& archive)
    {
        archive(batchSource);
        archive(constBatchSource);
    }

    template<class ShardT>
    std::vector<::Inscription::Type> ReliquaryShards::Handler<ShardT>::InscriptionTypes(Inscription::BinaryArchive& archive) const
    {
        return ::Inscription::InputTypesFor<std::remove_const_t<ShardT>>(archive);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::CreateHandler()
    {
        handlers.push_back(std::make_unique<Handler<ShardT>>());
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

    template<class ShardT>
    ShardIndex<ShardT> ReliquaryShards::CreateCommon(RelicID id)
    {
        const auto type = TypeFor<ShardT>();
        if (Contains(Handle(id, Owner(), { type.name, true }, HandleObjectType::Shard)) ||
            Contains(Handle(id, Owner(), { type.name, false }, HandleObjectType::Shard)))
            throw CannotCreate(type);

        auto handler = FindHandler<ShardT>();
        if (!handler)
            throw NotRegistered(type, typeid(ShardT));
        handler->Create(id, Owner(), std::is_const_v<ShardT>);

        return CreateIndex<ShardT>(id);
    }

    template<class T>
    auto ReliquaryShards::CreateIndex(RelicID id) const
    {
        return ToIndex<T>(id, const_cast<Reliquary&>(Owner()));
    }
}