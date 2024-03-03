#pragma once

#include "ReliquaryShards.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT>
    Ptr<ShardT> ReliquaryShards::Create(RelicID id)
    {
        Relics().ModificationRequired(id);

        auto& batch = RequiredBatchSource<ShardT>();
        auto added = batch.Add(id);
        Owner().Raise<Created>(HandleFrom(id));
        return PtrFrom<ShardT>(id);
    }

    template<class ShardT>
    void ReliquaryShards::Destroy(RelicID id)
    {
        Relics().ModificationRequired(id);

        auto& batch = RequiredBatchSource<ShardT>();
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
        auto& batch = RequiredBatchSource<ShardT>();
        return batch.Find(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    BatchSource<ShardT>* ReliquaryShards::FindBatchSource()
    {
        auto& sources = ShardSourceMapFor<ShardT>();
        auto found = sources.find(TypeHandleFor<ShardT>().name);
        if (found == sources.end())
            return nullptr;

        return static_cast<BatchSource<ShardT>*>(found->second.get());
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    BatchSource<ShardT>& ReliquaryShards::RequiredBatchSource()
    {
        const auto typeHandle = TypeHandleFor<ShardT>();
        auto found = FindBatchSource<ShardT>();
        if (!found)
            throw NotRegistered(typeHandle, typeid(ShardT));

        return *found;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && !std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::ShardSourceMapFor() -> BatchSourceMap&
    {
        return batchSources;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::ShardSourceMapFor() -> BatchSourceMap&
    {
        return constBatchSources;
    }
}