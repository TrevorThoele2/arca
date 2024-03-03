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
        Owner().Raise<Created>(HandleFrom(id));
        return PtrFrom<ShardT>(id);
    }

    template<class ShardT>
    void ReliquaryShards::Destroy(RelicID id)
    {
        Relics().ModificationRequired(id);

        auto& batch = batchSources.Required<ShardT>();
        batch.Destroy(id);
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
}