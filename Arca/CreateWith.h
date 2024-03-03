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

        template<class... InitializeArgs>
        explicit CreateWith(const RelicStructure& structure, InitializeArgs&& ... initializeArgs)
        {
            function =
                [structure, args = std::make_tuple(std::forward<InitializeArgs>(initializeArgs)...)](ReliquaryRelics& relics) mutable
            {
                return std::apply(
                    [structure, &relics](auto&& ... args)
                    {
                        return relics.template CreateWith<T>(structure, std::forward<InitializeArgs>(args)...);
                    },
                    args);
            };
        }

        template<class... InitializeArgs>
        explicit CreateWith(const std::string& structureName, InitializeArgs&& ... initializeArgs)
        {
            function =
                [structureName, args = std::make_tuple(std::forward<InitializeArgs>(initializeArgs)...)](ReliquaryRelics& relics) mutable
            {
                return std::apply(
                    [structureName, &relics](auto&& ... args)
                    {
                        return relics.template CreateWith<T>(structureName, std::forward<InitializeArgs>(args)...);
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