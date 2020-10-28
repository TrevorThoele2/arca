#pragma once

#include <type_traits>
#include "RelicInit.h"

namespace Arca
{
    template<class T, class Enable = void>
    struct has_relic_init_constructor : std::false_type
    {};

    template<class T>
    struct has_relic_init_constructor<T, std::void_t<decltype(void(T{ std::declval<RelicInit>() }))>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_relic_init_constructor_v = has_relic_init_constructor<T>::value;
}