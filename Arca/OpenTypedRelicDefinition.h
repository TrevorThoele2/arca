#pragma once

#include "OpenTypedRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* OpenTypedRelic::Create()
    {
        return owner->shards.Create<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void OpenTypedRelic::Destroy()
    {
        owner->shards.Destroy(AsHandle<ShardT>(ID(), Owner()));
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* OpenTypedRelic::Find() const
    {
        return owner->Find<ShardT>(id);
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    typename EitherT::ShardT* OpenTypedRelic::Find() const
    {
        return owner->Find<EitherT>(id);
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

    template<class... ShardsT, std::enable_if_t<are_all_shards_v<ShardsT...> && (sizeof...(ShardsT) > 1), int>>
    bool OpenTypedRelic::Contains() const
    {
        return owner->Contains<ShardsT...>(id);
    }
}