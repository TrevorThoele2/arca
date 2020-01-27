#pragma once

#include <type_traits>

namespace Arca
{
    template <class T, class = void>
    struct has_bare_initialize_method : std::false_type
    {};

    template <class T>
    struct has_bare_initialize_method<T, std::void_t<decltype(std::declval<T>().Initialize())>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_bare_initialize_method_v = has_bare_initialize_method<T>::value;
}