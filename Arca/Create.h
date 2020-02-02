#pragma once

#include "Command.h"

#include <functional>
#include "IsRelic.h"
#include "IsShard.h"
#include "RelicID.h"
#include "RelicIndex.h"
#include "ShardIndex.h"

namespace Arca
{
    class ReliquaryRelics;
    class ReliquaryShards;

    template<class T, class Enable = void>
    struct Create;

    template<class T>
    struct Create<T, std::enable_if_t<is_relic_v<T>>>
    {
        std::function<RelicIndex<T>(ReliquaryRelics&)> function;

        template<class... ConstructorArgs>
        explicit Create(ConstructorArgs&& ... constructorArgs)
        {
            function =
                [args = std::make_tuple(std::forward<ConstructorArgs>(constructorArgs)...)] (ReliquaryRelics& relics) mutable
            {
                return std::apply(
                    [&relics](auto&& ... args)
                    {
                        return relics.template Create<T>(std::forward<ConstructorArgs>(args)...);
                    },
                    args);
            };
        }
    };

    template<class T>
    struct Traits<Create<T, std::enable_if_t<is_relic_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Create<Relic>";
        using Return = RelicIndex<T>;
    };

    template<class T>
    struct Create<T, std::enable_if_t<is_shard_v<T>>>
    {
        std::function<ShardIndex<T>(ReliquaryShards&)> function;

        template<class... ConstructorArgs>
        explicit Create(RelicID id, ConstructorArgs&& ... constructorArgs)
        {
            function =
                [id, args = std::make_tuple(std::forward<ConstructorArgs>(constructorArgs)...)](ReliquaryShards& shards) mutable
            {
                return std::apply(
                    [id, &shards](auto&& ... args)
                    {
                        return shards.template Create<T>(id, std::forward<ConstructorArgs>(args)...);
                    },
                    args);
            };
        }
    };

    template<class T>
    struct Traits<Create<T, std::enable_if_t<is_shard_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Create<Shard>";
        using Return = ShardIndex<T>;
    };
}