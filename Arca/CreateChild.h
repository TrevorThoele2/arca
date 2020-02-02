#pragma once

#include "Command.h"

#include <functional>
#include "IsRelic.h"
#include "Handle.h"
#include "RelicIndex.h"

namespace Arca
{
    class ReliquaryRelics;

    template<class T>
    struct CreateChild
    {
        std::function<RelicIndex<T>(ReliquaryRelics&)> function;

        template<class... ConstructorArgs>
        explicit CreateChild(const Handle& parent, ConstructorArgs&& ... constructorArgs)
        {
            function =
                [parent, args = std::make_tuple(std::forward<ConstructorArgs>(constructorArgs)...)](ReliquaryRelics& relics) mutable
            {
                return std::apply(
                    [parent, &relics](auto&& ... args)
                    {
                        return relics.template CreateChild<T>(parent, std::forward<ConstructorArgs>(args)...);
                    },
                    args);
            };
        }

        static_assert(is_relic_v<T>, "CreateChild can only be used with relics.");
    };

    template<class T>
    struct Traits<CreateChild<T>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "CreateChild";
        using Return = RelicIndex<T>;
    };
}