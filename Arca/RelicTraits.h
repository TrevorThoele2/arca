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

    template <class T, class = void>
    struct relic_has_shards : std::false_type
    {};

    template <class T>
    struct relic_has_shards<T, std::void_t<typename RelicTraits<T>::Shards>> : std::true_type
    {};

    template<class T>
    static constexpr bool relic_has_shards_v = relic_has_shards<T>::value;

    template<class T, bool>
    class DiscoverShards;

    template<class T>
    class DiscoverShards<T, true>
    {
    public:
        using Type = typename RelicTraits<T>::Shards;
    };

    template<class T>
    class DiscoverShards<T, false>
    {
    public:
        using Type = ShardList<>;
    };

    template<class T>
    using ShardsFor = typename DiscoverShards<T, relic_has_shards_v<T>>::Type;
}
