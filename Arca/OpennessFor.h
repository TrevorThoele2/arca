#pragma once

#include <type_traits>

#include "IsOpen.h"

namespace Arca
{
    template<class T, std::enable_if_t<is_open_v<T>, int> = 0>
    constexpr Openness OpennessFor()
    {
        return Openness::Open;
    }

    template<class T, std::enable_if_t<!is_open_v<T>, int> = 0>
    constexpr Openness OpennessFor()
    {
        return Openness::Closed;
    }
}