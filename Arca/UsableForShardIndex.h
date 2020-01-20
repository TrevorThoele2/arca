#pragma once

#include "IsShard.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_shard_index_v = is_shard_v<T>;
}