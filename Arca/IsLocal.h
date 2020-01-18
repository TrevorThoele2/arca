#pragma once

#include <type_traits>

#include "HasLocalityTrait.h"
#include "Locality.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct is_local : std::false_type
    {};

    template <class T>
    struct is_local<T, std::enable_if_t<!has_locality_trait_v<T>>> : std::true_type
    {};

    template <class T>
    struct is_local<T, std::enable_if_t<Traits<T>::locality == Locality::Local>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_local_v = is_local<T>::value;
}