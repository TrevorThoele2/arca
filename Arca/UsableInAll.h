#pragma once

#include "IsShard.h"
#include "IsMatrix.h"
#include <Chroma/Iterate.h>

namespace Arca
{
    template<::Chroma::VariadicTemplateSize i>
    struct UsableInAllIterator
    {
        template<class ShardPack>
        static constexpr bool Do(ShardPack pack)
        {
            using T = typename ShardPack::template Parameter<i - 1>::Type;
            const auto value = is_shard_v<T> || is_matrix_v<T>;
            return value ? UsableInAllIterator<i - 1>::Do(pack) : false;
        }
    };

    template<>
    struct UsableInAllIterator<0>
    {
        template<class ShardPack>
        static constexpr bool Do(ShardPack)
        {
            using T = typename ShardPack::template Parameter<0>::Type;
            return is_shard_v<T> || is_matrix_v<T>;
        }
    };

    template<class... Shards>
    struct usable_in_all : std::bool_constant<
        sizeof...(Shards) == 0
        || UsableInAllIterator<sizeof...(Shards) - 1>::Do(::Chroma::VariadicTemplate<Shards...>{})>
    {};

    template<class... Shards>
    static constexpr bool usable_in_all_v = usable_in_all<Shards...>::value;
}