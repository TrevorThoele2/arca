#pragma once

#include "IsRelic.h"
#include "IsLocal.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_local_ptr_v =
        (is_relic_v<T> && is_local_v<T>) || is_shard_v<T>;
}
