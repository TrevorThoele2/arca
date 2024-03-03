#pragma once

#include "Traits.h"

namespace Arca
{
    template <class T, class = void>
    struct is_signal : std::false_type
    {};

    template <class T>
    struct is_signal<T, std::enable_if_t<Traits<std::decay_t<T>>::objectType == ObjectType::Signal>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_signal_v = is_signal<T>::value;
}