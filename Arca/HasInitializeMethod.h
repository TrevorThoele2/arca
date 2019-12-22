#pragma once

#include <type_traits>

namespace Arca
{
    class Reliquary;

    template <class T, class = void>
    struct has_initialize_method : std::false_type
    {};

    template <class T>
    struct has_initialize_method<T, std::void_t<decltype(&T::Initialize)>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_initialize_method_v = has_initialize_method<T>::value;
}