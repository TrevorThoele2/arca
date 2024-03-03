#pragma once

#include <type_traits>

#include "Traits.h"

namespace Arca
{
    template<class T, class Enable = void>
    struct has_self_contained_trait : std::false_type
    {};

    template<class T>
    struct has_self_contained_trait<T, std::void_t<decltype(Traits<T>::selfContained)>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_self_contained_trait_v = has_self_contained_trait<T>::value;
}