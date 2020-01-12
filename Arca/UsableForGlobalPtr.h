#pragma once

#include "IsRelic.h"
#include "IsGlobal.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_global_ptr_v = is_relic_v<T> && is_global_v<T>;
}
