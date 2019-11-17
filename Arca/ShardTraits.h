#pragma once

#include "TypeHandle.h"

namespace Arca
{
    template<class T>
    struct ShardTraits;

    template <class T, class = void>
    struct is_shard : std::false_type
    {};

    template <class T>
    struct is_shard<T, std::void_t<decltype(&ShardTraits<std::decay_t<T>>::typeHandle)>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_shard_v = is_shard<T>::value;

    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    static constexpr TypeHandle TypeHandleFor()
    {
        return ShardTraits<T>::typeHandle;
    }
}