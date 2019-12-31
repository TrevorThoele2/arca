#pragma once

#include "ClosedRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    LocalPtr<ShardT> ClosedRelic::Find() const
    {
        return Arca::LocalPtr<ShardT>(id, Owner());
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    LocalPtr<EitherT> ClosedRelic::Find() const
    {
        return Arca::LocalPtr<EitherT>(id, Owner());
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool ClosedRelic::Contains() const
    {
        return static_cast<bool>(Find<ShardT>());
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    bool ClosedRelic::Contains() const
    {
        return owner->Contains<EitherT>(id);
    }

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    bool ClosedRelic::Contains() const
    {
        return owner->Contains<ShardsT>(id);
    }
}