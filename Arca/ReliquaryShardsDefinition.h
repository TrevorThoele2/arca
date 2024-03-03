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

        auto& batch = batchSources.Required<ShardT>();
        batch.Destroy(id);
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
}