#pragma once

#include "Traits.h"

namespace Arca
{
    template<class T, std::enable_if_t<!std::is_const_v<T>, int> = 0>
    Type TypeFor()
    {
        return { Traits<std::decay_t<T>>::TypeName(), false };
    }

    template<class T, std::enable_if_t<std::is_const_v<T>, int> = 0>
    Type TypeFor()
    {
        return { Traits<std::decay_t<T>>::TypeName(), true };
    }
}