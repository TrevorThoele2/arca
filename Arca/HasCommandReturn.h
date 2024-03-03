#pragma once

#include <type_traits>
#include "Traits.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct has_command_return : std::false_type
    {};

    template <class T>
    struct has_command_return<T, std::void_t<typename Traits<T>::Return>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_command_return_v = has_command_return<T>::value;
}