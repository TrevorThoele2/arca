#pragma once

#include "FixedRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Ptr<ShardT> FixedRelic::Find() const
    {
        return owner->Find<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool FixedRelic::Has() const
    {
        return static_cast<bool>(Find<ShardT>());
    }
}