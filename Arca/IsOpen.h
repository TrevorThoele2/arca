#pragma once

#include <type_traits>

namespace Arca
{
    template <class T, class Enable = void>
    struct is_open : std::false_type
    {};

    template <class T>
    struct is_open<T, std::enable_if_t<Traits<T>::openness == Openness::Open>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_open_v = is_open<T>::value;
}