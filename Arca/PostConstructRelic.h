#pragma once

#include "HasPostConstructMethod.h"
#include "HasRequiredShards.h"
#include "ExtractShards.h"

namespace Arca
{
    template<
        class T,
        std::enable_if_t<
            has_post_construct_method_v<T>
            && has_required_shards_method_v<T>
            && (std::tuple_size_v<ShardTuple<typename T::Shards>> > 0), int> = 0>
    void PostConstructRelic(T& relic)
    {
        auto shards = ExtractShards<typename T::Shards>(relic.ID(), relic.Owner());
        relic.PostConstruct(shards);
    }

    template<
        class T,
        std::enable_if_t<
            has_post_construct_method_v<T>
            && (!has_required_shards_method_v<T> || (std::tuple_size_v<ShardTuple<typename T::Shards>> == 0)), int> = 0>
    void PostConstructRelic(T& relic)
    {
        relic.PostConstruct();
    }

    template<class T, std::enable_if_t<!has_post_construct_method_v<T>, int> = 0>
    void PostConstructRelic(T& relic)
    {}
}