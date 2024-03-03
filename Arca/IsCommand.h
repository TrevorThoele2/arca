#pragma once

#include <type_traits>
#include "Traits.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct is_command : std::false_type
    {};

    template <class T>
    struct is_command<T, std::enable_if_t<Traits<T>::objectType == ObjectType::Command>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_command_v = is_command<T>::value;
}