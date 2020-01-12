#pragma once

#include "OpenTypedRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    LocalPtr<ShardT> OpenTypedRelic::Create()
    {
        return owner->shards.Create<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void OpenTypedRelic::Destroy()
    {
        owner->shards.Destroy<ShardT>(ID());
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    LocalPtr<ShardT> OpenTypedRelic::Find() const
    {
        return Arca::LocalPtr<ShardT>(id, *owner);
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    LocalPtr<EitherT> OpenTypedRelic::Find() const
    {
        return Arca::LocalPtr<EitherT>(id, *owner);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool OpenTypedRelic::Contains() const
    {
        return static_cast<bool>(Find<ShardT>());
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    bool OpenTypedRelic::Contains() const
    {
        return owner->Contains<EitherT>(id);
    }

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    bool OpenTypedRelic::Contains() const
    {
        return owner->Contains<ShardsT>(id);
    }
}