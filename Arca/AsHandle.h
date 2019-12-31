#pragma once

#include "IsRelic.h"
#include "IsShard.h"
#include "TypeFor.h"

namespace Arca
{
    class Reliquary;

    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    Handle AsHandle(const T& object)
    {
        return Handle(object.ID(), object.Owner(), TypeFor<T>(), HandleObjectType::Relic);
    }

    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    Handle AsHandle(RelicID id, Reliquary& owner)
    {
        return Handle(id, owner, TypeFor<T>(), HandleObjectType::Shard);
    }
}