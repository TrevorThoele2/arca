#pragma once

#include <type_traits>

namespace Arca
{
    class Reliquary;

    template <class T, class = void>
    struct has_post_construct_method : std::false_type
    {};

    template <class T>
    struct has_post_construct_method<T, std::void_t<decltype(&T::PostConstruct)>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_post_construct_method_v = has_post_construct_method<T>::value;
}