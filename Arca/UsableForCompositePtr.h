#pragma once

#include "IsComposite.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_composite_ptr_v = is_composite_v<T>;
}
