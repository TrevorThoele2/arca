#pragma once

#include <type_traits>

#include "HasSelfContainedTrait.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct is_self_contained : std::false_type
    {};

    template <class T>
    struct is_self_contained<T, std::enable_if_t<!has_self_contained_trait_v<T>>> : std::false_type
    {};

    template <class T>
    struct is_self_contained<T, std::enable_if_t<Traits<T>::selfContained>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_self_contained_v = is_self_contained<T>::value;
}