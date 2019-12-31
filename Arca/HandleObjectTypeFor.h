#pragma once

#include "Handle.h"

#include "IsRelic.h"
#include "IsShard.h"

namespace Arca
{
    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    HandleObjectType HandleObjectTypeFor()
    {
        return HandleObjectType::Relic;
    }

    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    HandleObjectType HandleObjectTypeFor()
    {
        return HandleObjectType::Shard;
    }
}
