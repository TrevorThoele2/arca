#pragma once

#include "TypeHandle.h"

#include "Chroma/VariadicTemplate.h"

namespace Arca
{
    template<class T>
    struct RelicTraits;

    template <class T, class = void>
    struct is_relic : std::false_type
    {};

    template <class T>
    struct is_relic<T, std::void_t<decltype(&RelicTraits<std::decay_t<T>>::typeHandle)>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_relic_v = is_relic<T>::value;

    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    static constexpr TypeHandle TypeHandleFor()
    {
        return RelicTraits<T>::typeHandle;
    }

    template<class... Args>
    using ShardList = Chroma::VariadicTemplate<Args...>;

    template<class T>
    using ShardsFor = typename RelicTraits<T>::Shards;
}
