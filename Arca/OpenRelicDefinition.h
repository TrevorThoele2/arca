#pragma once

#include "OpenRelic.h"
#include "Reliquary.h"
#include "AsHandle.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    LocalPtr<ShardT> OpenRelic::Create()
    {
        return owner->shards.Create<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void OpenRelic::Destroy()
    {
        owner->shards.Destroy<ShardT>(id);
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    void OpenRelic::Destroy()
    {
        owner->shards.Destroy<EitherT>(id);
    }

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    void OpenRelic::Destroy()
    {
        owner->shards.Destroy<ShardsT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    LocalPtr<ShardT> OpenRelic::Find() const
    {
        return Arca::LocalPtr<ShardT>(id, Owner());
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    LocalPtr<EitherT> OpenRelic::Find() const
    {
        return Arca::LocalPtr<EitherT>(id, Owner());
    }

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    CompositePtr<ShardsT> OpenRelic::Find() const
    {
        return Arca::CompositePtr<ShardsT>(id, Owner());
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

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    bool OpenRelic::Contains() const
    {
        return owner->Contains<ShardsT>(id);
    }
}