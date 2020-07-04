#pragma once

#include "RelicID.h"

#include "ShardIndex.h"

namespace Arca
{
    class Reliquary;

    struct RelicInit
    {
        RelicID id;
        Reliquary& owner;

        template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Index<ShardT> Create(ConstructorArgs&& ... constructorArgs) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Index<ShardT> Find() const;
    };
}