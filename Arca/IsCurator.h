#pragma once

#include <type_traits>

#include "Traits.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct is_curator : std::false_type
    {};

    template <class T>
    struct is_curator<T, std::enable_if_t<Traits<T>::objectType == ObjectType::Curator>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_curator_v = is_curator<T>::value;
}
