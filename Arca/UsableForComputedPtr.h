#pragma once

#include "IsRelic.h"
#include "IsShard.h"
#include "IsMatrix.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_computed_ptr_v =
        !is_relic_v<T> && !is_shard_v<T> && !is_matrix_v<T>;
}