#pragma once

#include "TypeHandle.h"

namespace Arca
{
    template<class T>
    struct CuratorTraits;

    template <class T, class = void>
    struct is_curator : std::false_type
    {};

    template <class T>
    struct is_curator<T, std::void_t<decltype(&CuratorTraits<std::decay_t<T>>::typeHandle)>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_curator_v = is_curator<T>::value;

    template<class T, std::enable_if_t<is_curator_v<T>, int> = 0>
    static constexpr TypeHandle TypeHandleFor()
    {
        return CuratorTraits<T>::typeHandle;
    }
}
