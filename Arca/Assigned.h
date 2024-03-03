#pragma once

#include "Traits.h"
#include "Handle.h"
#include "Index.h"
#include "TypeFor.h"

#include "IsRelic.h"
#include "IsShard.h"

namespace Arca
{
    template<class T>
    struct AssignedKnown
    {
        using ReferenceType = typename ReferenceTypeFor<T>::Type;
        
        ReferenceType reference;
    private:
        static_assert(is_relic_v<T> || is_shard_v<T>, "AssignedKnown must be used with relics or shards.");
    };

    template<class T>
    struct Traits<AssignedKnown<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "Arca::AssignedKnown<" + Traits<std::decay_t<T>>::typeName + ">";
    };
}