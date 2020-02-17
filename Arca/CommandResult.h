#pragma once

#include <type_traits>
#include "Traits.h"
#include "HasCommandResult.h"

namespace Arca
{
    template<class T, class Enable = void>
    struct command_result
    {
        using Type = void;
    };

    template<class T>
    struct command_result<T, std::enable_if_t<has_command_result_v<T>>>
    {
        using Type = typename Traits<T>::Result;
    };

    template<class T>
    using command_result_t = typename command_result<T>::Type;
}