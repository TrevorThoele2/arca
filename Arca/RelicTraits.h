#pragma once

#include "Traits.h"

#include <Chroma/VariadicTemplate.h>

namespace Arca
{
    template <class T, class = void>
    struct is_relic : std::false_type
    {};

    template <class T>
    struct is_relic<T, std::enable_if_t<Traits<std::decay_t<T>>::objectType == ObjectType::Relic>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_relic_v = is_relic<T>::value;

    template<class... Args>
    using ShardList = Chroma::VariadicTemplate<Args...>;

    template <class T, class = void>
    struct has_shards : std::false_type
    {};

    template <class T>
    struct has_shards<T, std::void_t<typename Traits<T>::Shards>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_shards_v = has_shards<T>::value;

    template<class T, bool>
    class DiscoverShards;

    template<class T>
    class DiscoverShards<T, true>
    {
    public:
        using Type = typename Traits<T>::Shards;
    };

    template<class T>
    class DiscoverShards<T, false>
    {
    public:
        using Type = ShardList<>;
    };

    template<class T>
    using shards_for = DiscoverShards<T, has_shards_v<T>>;

    template<class T>
    using shards_for_t = typename shards_for<T>::Type;
}
