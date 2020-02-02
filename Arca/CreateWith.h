#pragma once

#include "Command.h"

#include <functional>
#include "IsRelic.h"
#include "RelicStructure.h"
#include "RelicIndex.h"

namespace Arca
{
    class ReliquaryRelics;

    template<class T>
    struct CreateWith
    {
        std::function<RelicIndex<T>(ReliquaryRelics&)> function;

        template<class... ConstructorArgs>
        explicit CreateWith(const RelicStructure& structure, ConstructorArgs&& ... constructorArgs)
        {
            function =
                [structure, args = std::make_tuple(std::forward<ConstructorArgs>(constructorArgs)...)](ReliquaryRelics& relics) mutable
            {
                return std::apply(
                    [structure, &relics](auto&& ... args)
                    {
                        return relics.template CreateWith<T>(structure, std::forward<ConstructorArgs>(args)...);
                    },
                    args);
            };
        }

        template<class... ConstructorArgs>
        explicit CreateWith(const std::string& structureName, ConstructorArgs&& ... constructorArgs)
        {
            function =
                [structureName, args = std::make_tuple(std::forward<ConstructorArgs>(constructorArgs)...)](ReliquaryRelics& relics) mutable
            {
                return std::apply(
                    [structureName, &relics](auto&& ... args)
                    {
                        return relics.template CreateWith<T>(structureName, std::forward<ConstructorArgs>(args)...);
                    },
                    args);
            };
        }

        static_assert(is_relic_v<T>, "CreateWith can only be used with relics.");
    };

    template<class T>
    struct Traits<CreateWith<T>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "CreateWith";
        using Return = RelicIndex<T>;
    };
}