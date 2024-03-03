#pragma once

#include <type_traits>

namespace Arca
{
    class Reliquary;

    template <class T, class Enable = void>
    struct has_should_create_method : std::false_type
    {};

    template <class T>
    struct has_should_create_method<T, std::void_t<decltype(Traits<T>::ShouldCreate)>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_should_create_method_v = has_should_create_method<T>::value;
}