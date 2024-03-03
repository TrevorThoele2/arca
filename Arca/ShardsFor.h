#pragma once

#include "HasShards.h"
#include "ShardList.h"

namespace Arca
{
    template<class T, bool>
    class DiscoverShards;

    template<class T>
    class DiscoverShards<T, true>
    {
    public:
        using Type = typename T::Shards;
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