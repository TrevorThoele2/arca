#pragma once

#include "DynamicRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> DynamicRelic::Create()
    {
        return owner->shards.Create<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void DynamicRelic::Destroy()
    {
        owner->shards.Destroy<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> DynamicRelic::Find() const
    {
        return owner->Find<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool DynamicRelic::Has() const
    {
        return static_cast<bool>(Find<ShardT>());
    }
}