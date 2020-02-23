#pragma once

#include "Command.h"

#include "IsRelic.h"
#include "IsShard.h"
#include "IsMatrix.h"
#include "RelicID.h"
#include "Index.h"

namespace Arca
{
    class ReliquaryRelics;
    class ReliquaryShards;

    template<class T, class Enable = void>
    struct Destroy;

    template<class T>
    struct Destroy<T, std::enable_if_t<is_relic_v<T>>>
    {
        RelicID id;

        explicit Destroy(RelicID id) : id(id)
        {}

        explicit Destroy(const T& relic) : id(relic.id)
        {}

        explicit Destroy(Index<T> index) : id(index.ID())
        {}
    };

    template<class T>
    struct Traits<Destroy<T, std::enable_if_t<is_relic_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Destroy<Relic>";
    };

    template<class T>
    struct Destroy<T, std::enable_if_t<is_shard_v<T>>>
    {
        RelicID id;

        explicit Destroy(RelicID id) : id(id)
        {}
    };

    template<class T>
    struct Traits<Destroy<T, std::enable_if_t<is_shard_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Destroy<Shard>";
    };

    template<class T>
    struct Destroy<T, std::enable_if_t<is_matrix_v<T>>>
    {
        RelicID id;

        explicit Destroy(RelicID id) : id(id)
        {}
    };

    template<class T>
    struct Traits<Destroy<T, std::enable_if_t<is_matrix_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Destroy<Matrix>";
    };

    template<>
    struct Destroy<Handle>
    {
        Handle handle;

        explicit Destroy(Handle handle) : handle(std::move(handle))
        {}
    };

    template<>
    struct Traits<Destroy<Handle>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Destroy<Handle>";
    };
}