#pragma once

namespace Arca
{
    template<class Shard>
    struct ShardCreated
    {
        using ShardT = Shard;
        ShardT& shard;
    };
}