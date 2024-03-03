#pragma once

#include <type_traits>
#include "All.h"

namespace Arca
{
    template<class... Ts>
    struct is_all : std::false_type
    {};

    template<class... Ts>
    struct is_all<All<Ts...>> : std::true_type
    {};

    template<class... Ts>
    static constexpr bool is_all_v = is_all<Ts...>::value;
}