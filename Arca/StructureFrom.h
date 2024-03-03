#pragma once

#include <Chroma/VariadicTemplate.h>
#include <Chroma/Iterate.h>
#include "RelicStructure.h"
#include "ShardTraits.h"

namespace Arca
{
    template<::Chroma::VariadicTemplateSize i>
    struct ShardListToStructure
    {
        template<class ShardList>
        constexpr static void Do(RelicStructure& structure, ShardList shardList)
        {
            using ShardT = typename ShardList::template Parameter<i - 1>::Type;

            auto typeHandle = TypeHandleFor<ShardT>();
            structure.push_back(typeHandle);
            ShardListToStructure<i - 1>::Do(structure, shardList);
        }
    };

    template<>
    struct ShardListToStructure<0>
    {
        template<class ShardList>
        constexpr static void Do(RelicStructure&, ShardList)
        {}
    };

    template<class Shards>
    static constexpr RelicStructure StructureFrom()
    {
        RelicStructure structure;
        ::Chroma::IterateRange<
            ::Chroma::VariadicTemplateSize,
            ShardListToStructure,
            Shards::count
        >(structure, Shards{});
        return structure;
    }
}