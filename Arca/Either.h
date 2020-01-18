#pragma once

#include <type_traits>
#include "IsShard.h"
#include "Traits.h"

namespace Arca
{
    class Reliquary;

    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    struct Either
    {
        using ShardT = const std::decay_t<T>;
        using BareT = std::decay_t<T>;
    };

    template<class T>
    struct Traits<Either<T>>
    {
        static const ObjectType objectType = ObjectType::Matrix;
        static inline const TypeName typeName = "Either<" + Traits<std::decay_t<T>>::typeName + ">";
    };
}