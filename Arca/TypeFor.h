#pragma once

#include "Traits.h"

namespace Arca
{
    template<class T, std::enable_if_t<!std::is_const_v<T>, int> = 0>
    constexpr Type TypeFor()
    {
        return { Traits<std::decay_t<T>>::typeName, false };
    }

    template<class T, std::enable_if_t<std::is_const_v<T>, int> = 0>
    constexpr Type TypeFor()
    {
        return { Traits<std::decay_t<T>>::typeName, true };
    }

    template<class T, std::enable_if_t<!std::is_const_v<T>, int> = 0>
    constexpr std::string FullTypeNameFor()
    {
        return Traits<std::decay_t<T>>::typeName;
    }

    template<class T, std::enable_if_t<std::is_const_v<T>, int> = 0>
    constexpr std::string FullTypeNameFor()
    {
        return std::string("const ") + Traits<std::decay_t<T>>::typeName;
    }
}