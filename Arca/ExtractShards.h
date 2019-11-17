#pragma once

#include "Reliquary.h"
#include <Chroma/Iterate.h>

namespace Arca
{
    template<class Shard>
    struct ShardListToTuple
    {
        using Type = Shard*;
    };

    template<class Shards>
    using ShardTuple = typename Shards::template Transform<ShardListToTuple>::Type::template TupleT;

    template<::Chroma::VariadicTemplateSize i>
    struct ShardListExtractor
    {
        template<class ShardTuple>
        constexpr static void Do(ShardTuple& tuple, RelicID id, Reliquary& reliquary)
        {
            using ShardT = std::remove_pointer_t<std::tuple_element_t<i - 1, ShardTuple>>;

            auto shard = reliquary.Find<ShardT>(id);
            std::get<i - 1>(tuple) = shard;
            return ShardListExtractor<i - 1>::Do(tuple, id, reliquary);
        }
    };

    template<>
    struct ShardListExtractor<0>
    {
        template<class ShardTuple>
        constexpr static void Do(ShardTuple&, RelicID, Reliquary&)
        {}
    };

    template<class Shards>
    static constexpr ShardTuple<Shards> ExtractShards(RelicID id, Reliquary& reliquary)
    {
        auto shardTuple = ShardTuple<Shards>{};
        ::Chroma::IterateRange<
            ::Chroma::VariadicTemplateSize,
            ShardListExtractor,
            Shards::count
        >(shardTuple, id, reliquary);
        return shardTuple;
    }
}