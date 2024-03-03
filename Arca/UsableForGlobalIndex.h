#pragma once

#include "IsRelic.h"
#include "IsGlobal.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_global_index_v = is_global_v<T>;
}
