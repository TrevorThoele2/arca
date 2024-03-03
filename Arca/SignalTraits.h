#pragma once

#include "Traits.h"
#include "ObjectType.h"

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

    template<class T, std::enable_if_t<is_signal_v<T>, int> = 0>
    ObjectType ObjectTypeFor()
    {
        return ObjectType::Signal;
    }
}