#pragma once

#include "Traits.h"
#include "Handle.h"
#include "LocalPtr.h"
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
        typename PtrTypeFor<T>::Type ptr;

        static_assert(is_relic_v<T> || is_shard_v<T>, "CreatedKnown must be used for relics or shards.");
    };

    template<>
    struct Traits<Destroying>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "Destroying";
    };

    template<class T>
    struct Traits<DestroyingKnown<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "DestroyingKnown<" + ::Chroma::ToString(TypeFor<T>()) + ">";
    };
}
