#pragma once

#include <type_traits>

#include "Traits.h"

namespace Arca
{
    template<class T, class Enable = void>
    struct has_openness_trait : std::false_type
    {};

    template<class T>
    struct has_openness_trait<T, std::void_t<decltype(Traits<T>::openness)>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_openness_trait_v = has_openness_trait<T>::value;
}