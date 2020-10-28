#pragma once

#include <type_traits>

#include "HasOpennessTrait.h"
#include "Openness.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct is_closed : std::false_type
    {};

    template <class T>
    struct is_closed<T, std::enable_if_t<!has_openness_trait_v<T>>> : std::true_type
    {};

    template <class T>
    struct is_closed<T, std::enable_if_t<Traits<T>::openness == Openness::Closed>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_closed_v = is_closed<T>::value;
}