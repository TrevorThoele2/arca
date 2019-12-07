#pragma once

#include "Traits.h"
#include "Handle.h"

namespace Arca
{
    template <class T, class = void>
    struct is_shard : std::false_type
    {};

    template <class T>
    struct is_shard<T, std::enable_if_t<Traits<std::decay_t<T>>::objectType == ObjectType::Shard>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_shard_v = is_shard<T>::value;

    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    ObjectType ObjectTypeFor()
    {
        return ObjectType::Shard;
    }

    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    HandleObjectType HandleObjectTypeFor()
    {
        return HandleObjectType::Relic;
    }

    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    Handle AsHandle(RelicID id, Reliquary& owner)
    {
        return Handle(id, owner, TypeFor<T>(), HandleObjectType::Shard);
    }
}