#pragma once

#include <type_traits>

#include "Traits.h"

namespace Arca
{
    template<class T, class Enable = void>
    struct has_locality_trait : std::false_type
    {};

    template<class T>
    struct has_locality_trait<T, std::void_t<decltype(Traits<T>::locality)>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_locality_trait_v = has_locality_trait<T>::value;
}