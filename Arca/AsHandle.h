#pragma once

#include "IsRelic.h"
#include "IsShard.h"
#include "TypeFor.h"

namespace Arca
{
    class Reliquary;

    template<class T, std::enable_if_t<is_relic_v<T> || is_shard_v<T>, int> = 0>
    Handle AsHandle(Index<T> index)
    {
        return Handle{ index.ID(), TypeFor<T>() };
    }

    template<class T, std::enable_if_t<is_relic_v<T> || is_shard_v<T>, int> = 0>
    Handle AsHandle(RelicID id)
    {
        return Handle{ id, TypeFor<T>() };
    }
}