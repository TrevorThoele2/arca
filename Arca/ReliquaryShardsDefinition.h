#pragma once

#include "ReliquaryShards.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT>
    Ptr<ShardT> ReliquaryShards::Create(RelicID id)
    {
        Relics().ModificationRequired(id);

        auto& batch = batchSources.Required<ShardT>();
        auto added = batch.Add(id);

        AttemptAddToEitherBatches(id, *added);
        NotifyCompositesShardCreate(id);

        Owner().Raise<Created>(HandleFrom(id));
        return PtrFrom<ShardT>(id);
    }

    template<class ShardT>
    void ReliquaryShards::Destroy(RelicID id)
    {
        Relics().ModificationRequired(id);

        {
            auto eitherBatchSource = eitherBatchSources.Find<Either<std::decay_t<ShardT>>>();
            if (eitherBatchSource)
                eitherBatchSource->DestroyFromBase(id, std::is_const_v<ShardT>);
        }

        NotifyCompositesShardDestroy(id);
        auto& batch = batchSources.Required<ShardT>();
        batch.Destroy(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> ReliquaryShards::Find(RelicID id) const
    {
        auto& batchSource = batchSources.Required<ShardT>();
        auto found = batchSource.Find(id);
        if (!found)
            return {};

        return Ptr<ShardT>(id, const_cast<Reliquary&>(Owner()));
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        return static_cast<bool>(Find<ShardT>(id));
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        using ShardT = typename EitherT::BareT;
        return static_cast<bool>(Find<ShardT>(id))
            || static_cast<bool>(Find<const ShardT>(id));
    }

    template<class... ShardsT, std::enable_if_t<are_all_shards_v<ShardsT...> && (sizeof...(ShardsT) > 1), int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        return Contains<All<ShardsT...>>(id);
    }

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        return ::Chroma::IterateRangeCheckStop<
            ::Chroma::VariadicTemplateSize,
            ContainsAllShardsIterator,
            bool,
            ShardsT::Pack::count - 1>

            (false, typename ShardsT::Pack{}, id, *this);
    }

    template<class ShardT>
    void ReliquaryShards::AttemptAddToEitherBatches(RelicID id, ShardT& shard)
    {
        auto found = eitherBatchSources.Find<Either<ShardT>>();
        if (found)
            found->Add(id, shard, std::is_const_v<ShardT>);
    }

    template<class ShardT>
    auto ReliquaryShards::FindFactory() -> Factory
    {
        auto typeHandle = TypeHandleFor<ShardT>();
        const auto found = factoryMap.find(typeHandle);
        if (found == factoryMap.end())
            return nullptr;

        return found->second;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* ReliquaryShards::FindStorage(RelicID id)
    {
        auto& batch = batchSources.Required<ShardT>();
        return batch.Find(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && !std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::BatchSources::MapFor() -> Map&
    {
        return map;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && !std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::BatchSources::MapFor() const -> const Map&
    {
        return map;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::BatchSources::MapFor() -> Map&
    {
        return constMap;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::BatchSources::MapFor() const -> const Map&
    {
        return constMap;
    }

    template<class T, std::enable_if_t<is_either_v<T>, int>>
    auto ReliquaryShards::EitherBatchSources::MapFor() -> Map&
    {
        return map;
    }

    template<class T, std::enable_if_t<is_either_v<T>, int>>
    auto ReliquaryShards::EitherBatchSources::MapFor() const -> const Map&
    {
        return map;
    }

    template<class T>
    std::unique_ptr<EitherShardBatchSourceBase> ReliquaryShards::EitherBatchSources::Create()
    {
        auto created = std::make_unique<BatchSource<T>>(*owner);
        for (auto& loop : owner->batchSources.Required<ShardT<T>>())
            created->Add(loop.id, loop.shard, false);
        for (auto& loop : owner->batchSources.Required<const ShardT<T>>())
            created->Add(loop.id, loop.shard, true);
        return created;
    }

    template<class T>
    TypeHandleName ReliquaryShards::EitherBatchSources::KeyFor()
    {
        return Arca::TypeHandleFor<ShardT<T>>().name;
    }

    template<class T, std::enable_if_t<is_composite_v<T>, int>>
    auto ReliquaryShards::CompositeBatchSources::MapFor() -> Map&
    {
        return map;
    }

    template<class T, std::enable_if_t<is_composite_v<T>, int>>
    auto ReliquaryShards::CompositeBatchSources::MapFor() const -> const Map&
    {
        return map;
    }

    template<class T>
    std::unique_ptr<CompositeShardBatchSourceBase> ReliquaryShards::CompositeBatchSources::Create()
    {
        auto created = std::make_unique<BatchSource<T>>(*owner);
        auto allIDs = owner->Owner().AllIDs();
        for (auto& loop : allIDs)
            if (owner->Owner().Contains<T>(loop))
                created->Add(loop);
        return created;
    }

    template<class T>
    [[nodiscard]] std::type_index ReliquaryShards::CompositeBatchSources::KeyFor()
    {
        return std::type_index(typeid(T));
    }
}