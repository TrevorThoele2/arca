#pragma once

namespace Arca
{
    template<class Shard>
    struct BeforeShardDestroyed
    {
        using ShardT = Shard;
        ShardT& shard;
    };
}