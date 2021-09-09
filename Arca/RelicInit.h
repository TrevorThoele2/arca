#pragma once

#include "RelicID.h"

#include "ShardIndex.h"
#include "ReliquaryShards.h"

namespace Arca
{
    class Reliquary;

    class RelicInit
    {
    public:
        RelicID id;
        Reliquary& owner;
    public:
        RelicInit(RelicID id, Reliquary& owner, ReliquaryShards& shards);

        template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Index<ShardT> Create(ConstructorArgs&& ... constructorArgs) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Index<ShardT> Find() const;
    private:
        ReliquaryShards* shards;
    };
}