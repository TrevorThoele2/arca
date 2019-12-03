#pragma once

#include "OpenRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> OpenRelic::Create()
    {
        return owner->shards.Create<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void OpenRelic::Destroy()
    {
        owner->shards.Destroy<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> OpenRelic::Find() const
    {
        return owner->Find<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool OpenRelic::Contains() const
    {
        return static_cast<bool>(Find<ShardT>());
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    bool OpenRelic::Contains() const
    {
        return owner->Contains<EitherT>(id);
    }
}