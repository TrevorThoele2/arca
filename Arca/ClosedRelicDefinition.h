#pragma once

#include "ClosedRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> ClosedRelic::Find() const
    {
        return owner->Find<ShardT>(id);
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    Ptr<typename EitherT::ShardT> ClosedRelic::Find() const
    {
        return owner->Find<EitherT>(id);
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

    template<class... ShardsT, std::enable_if_t<are_all_shards_v<ShardsT...> && (sizeof...(ShardsT) > 1), int>>
    bool ClosedRelic::Contains() const
    {
        return owner->Contains<ShardsT...>(id);
    }
}