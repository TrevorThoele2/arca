#pragma once

#include "Traits.h"
#include "Handle.h"
#include "RelicIndex.h"
#include "ShardIndex.h"
#include "TypeFor.h"

#include "IsRelic.h"
#include "IsShard.h"

namespace Arca
{
    struct Created
    {
        Handle handle;
    };

    template<class T>
    struct CreatedKnown
    {
        typename IndexTypeFor<T>::Type index;

        static_assert(is_relic_v<T> || is_shard_v<T>, "CreatedKnown must be used for relics or shards.");
    };

    template<>
    struct Traits<Created>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "Created";
    };

    template<class T>
    struct Traits<CreatedKnown<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "CreatedKnown<" + Traits<std::decay_t<T>>::typeName + ">";
    };
}
