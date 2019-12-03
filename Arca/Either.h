#pragma once

#include <type_traits>
#include "ShardTraits.h"

namespace Arca
{
    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    struct Either
    {
        using ShardT = const std::decay_t<T>;
        using BareT = std::decay_t<T>;
    };

    template<class T>
    struct is_either : std::false_type
    {};

    template<class T>
    struct is_either<Either<T>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_either_v = is_either<T>::value;
}