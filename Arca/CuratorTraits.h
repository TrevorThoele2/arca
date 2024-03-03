#pragma once

#include "Traits.h"

namespace Arca
{
    template <class T, class = void>
    struct is_curator : std::false_type
    {};

    template <class T>
    struct is_curator<T, std::enable_if_t<Traits<std::decay_t<T>>::objectType == ObjectType::Curator>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_curator_v = is_curator<T>::value;
}
