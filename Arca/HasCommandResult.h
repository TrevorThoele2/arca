#pragma once

#include <type_traits>
#include "Traits.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct has_command_result : std::false_type
    {};

    template <class T>
    struct has_command_result<
        T,
        std::enable_if_t<!std::is_void_v<typename Traits<T>::Result>>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_command_result_v = has_command_result<T>::value;
}