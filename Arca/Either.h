#pragma once

#include <type_traits>
#include "IsShard.h"
#include "Traits.h"

namespace Arca
{
    class Reliquary;

    template<class T>
    struct Either
    {
        using ShardT = const std::decay_t<T>;
        using BareT = std::decay_t<T>;

        static_assert(is_shard_v<T>, "Eithers must wrap a Shard");
    };

    template<class T>
    struct Traits<Either<T>>
    {
        static const ObjectType objectType = ObjectType::Matrix;
        static inline const TypeName typeName = "Arca::Either<" + Traits<std::decay_t<T>>::typeName + ">";
    };
}