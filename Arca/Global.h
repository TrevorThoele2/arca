#pragma once

#include "RelicTraits.h"

namespace Arca
{
    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    struct Global
    {
        using RelicT = T;
    };

    template<class T>
    struct is_global : std::false_type
    {};

    template<class T>
    struct is_global<Global<T>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_global_v = is_global<T>::value;
}
