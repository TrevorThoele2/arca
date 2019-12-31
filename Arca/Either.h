#pragma once

#include <type_traits>
#include "IsShard.h"

namespace Arca
{
    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    struct Either
    {
        using ShardT = const std::decay_t<T>;
        using BareT = std::decay_t<T>;
    };
}