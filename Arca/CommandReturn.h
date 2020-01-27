#pragma once

#include <type_traits>
#include "Traits.h"
#include "HasCommandReturn.h"

namespace Arca
{
    template<class T, class Enable = void>
    struct command_return
    {
        using Type = void;
    };

    template<class T>
    struct command_return<T, std::enable_if_t<has_command_return_v<T>>>
    {
        using Type = typename Traits<T>::Return;
    };

    template<class T>
    using command_return_t = typename command_return<T>::Type;
}