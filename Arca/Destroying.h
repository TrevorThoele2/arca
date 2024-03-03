#pragma once

#include "Traits.h"
#include "Handle.h"
#include "Index.h"
#include "TypeFor.h"

#include "IsRelic.h"
#include "IsShard.h"

namespace Arca
{
    struct Destroying
    {
        Handle handle;
    };

    template<class T>
    struct DestroyingKnown
    {
        Index<T> index;

        static_assert(is_relic_v<T> || is_shard_v<T>, "CreatedKnown must be used for relics or shards.");
    };

    template<>
    struct Traits<Destroying>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static constexpr TypeName TypeName() { return "Arca::Destroying"; }
    };

    template<class T>
    struct Traits<DestroyingKnown<T>>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static constexpr TypeName TypeName() { return "Arca::DestroyingKnown<" + FullTypeNameFor<T>() + ">"; }
    };
}
