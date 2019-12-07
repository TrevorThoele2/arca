#pragma once

#include "RelicStructure.h"
#include "ShardTraits.h"

#include <Chroma/VariadicTemplate.h>
#include <Chroma/Iterate.h>

namespace Arca
{
    template<::Chroma::VariadicTemplateSize i>
    struct ShardListToStructure
    {
        template<class ShardList>
        constexpr static void Do(RelicStructure& structure, ShardList shardList)
        {
            using ShardT = typename ShardList::template Parameter<i>::Type;

            structure.push_back(TypeFor<ShardT>());
        }
    };

    template<class Shards, std::enable_if_t<(Shards::count > 0), int> = 0>
    static constexpr RelicStructure StructureFrom()
    {
        RelicStructure structure;
        ::Chroma::IterateRange<::Chroma::VariadicTemplateSize, ShardListToStructure, Shards::count - 1>
            (structure, Shards{});
        return structure;
    }

    template<class Shards, std::enable_if_t<(Shards::count == 0), int> = 0>
    static constexpr RelicStructure StructureFrom()
    {
        return RelicStructure{};
    }
}