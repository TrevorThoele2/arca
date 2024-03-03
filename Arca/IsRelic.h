#pragma once

#include <type_traits>

#include "Traits.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct is_relic : std::false_type
    {};

    template <class T>
    struct is_relic<T, std::enable_if_t<Traits<T>::objectType == ObjectType::Relic>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_relic_v = is_relic<T>::value;
}