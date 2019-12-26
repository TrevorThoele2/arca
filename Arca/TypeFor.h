#pragma once

#include "Either.h"

namespace Arca
{
    template<class T, std::enable_if_t<!std::is_const_v<T> && !is_either_v<T>, int> = 0>
    Type TypeFor()
    {
        return { Traits<std::decay_t<T>>::typeName, false };
    }

    template<class T, std::enable_if_t<std::is_const_v<T> && !is_either_v<T>, int> = 0>
    Type TypeFor()
    {
        return { Traits<std::decay_t<T>>::typeName, true };
    }
}