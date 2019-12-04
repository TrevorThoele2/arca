#pragma once

#include "ShardTraits.h"
#include <Chroma/Iterate.h>

namespace Arca
{
    template<::Chroma::VariadicTemplateSize i>
    struct AreAllShardsIterator
    {
        template<class ShardPack>
        static constexpr bool Do(ShardPack pack)
        {
            using T = typename ShardPack::template Parameter<i>::Type;
            const auto value = is_shard_v<T>;
            return value ? AreAllShardsIterator<i - 1>::Do(pack) : false;
        }
    };

    template<>
    struct AreAllShardsIterator<0>
    {
        template<class ShardPack>
        static constexpr bool Do(ShardPack)
        {
            using T = typename ShardPack::template Parameter<0>::Type;
            return is_shard_v<T>;
        }
    };

    template<class... Shards>
    struct are_all_shards : std::bool_constant<
        AreAllShardsIterator<sizeof...(Shards) - 1>::Do(::Chroma::VariadicTemplate<Shards...>{})>
    {};

    template<class... Shards>
    static constexpr bool are_all_shards_v = are_all_shards<Shards...>::value;
}