#pragma once

#include <type_traits>
#include "RelicTraits.h"

namespace Arca
{
    class Reliquary;

    template <class T, class = void>
    struct has_factory_method : std::false_type
    {};

    template <class T>
    struct has_factory_method<T, std::void_t<decltype(RelicTraits<T>::Factory)>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_factory_method_v = has_factory_method<T>::value;
}