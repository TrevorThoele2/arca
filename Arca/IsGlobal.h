#pragma once

#include <type_traits>

#include "Locality.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct is_global : std::false_type
    {};

    template <class T>
    struct is_global<T, std::enable_if_t<Traits<T>::locality == Locality::Global>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_global_v = is_global<T>::value;
}