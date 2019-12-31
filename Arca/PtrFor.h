#pragma once

#include "LocalPtr.h"
#include "GlobalPtr.h"

namespace Arca
{
    template<class T, class = void>
    struct PtrFor;

    template<class T>
    struct PtrFor<T, std::enable_if_t<usable_for_local_ptr_v<T>>>
    {
        using Type = LocalPtr<T>;
    };

    template<class T>
    struct PtrFor<T, std::enable_if_t<usable_for_global_ptr_v<T>>>
    {
        using Type = GlobalPtr<T>;
    };
}