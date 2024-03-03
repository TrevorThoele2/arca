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

    template<class T, std::enable_if_t<!std::is_const_v<T>, int> = 0>
    std::string FullTypeNameFor()
    {
        return Traits<std::decay_t<T>>::TypeName();
    }

    template<class T, std::enable_if_t<std::is_const_v<T>, int> = 0>
    std::string FullTypeNameFor()
    {
        return std::string("const ") + Traits<std::decay_t<T>>::TypeName();
    }
}