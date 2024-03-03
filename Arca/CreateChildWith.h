#pragma once

#include "Command.h"

#include <functional>
#include "IsRelic.h"
#include "Handle.h"
#include "RelicStructure.h"
#include "RelicIndex.h"

namespace Arca
{
    class ReliquaryRelics;

    template<class T>
    struct CreateChildWith
    {
        std::function<RelicIndex<T>(ReliquaryRelics&)> function;

        template<class... InitializeArgs>
        explicit CreateChildWith(const Handle& parent, const RelicStructure& structure, InitializeArgs&& ... initializeArgs)
        {
            function =
                [parent, structure, args = std::make_tuple(std::forward<InitializeArgs>(initializeArgs)...)](ReliquaryRelics& relics) mutable
            {
                return std::apply(
                    [parent, structure, &relics](auto&& ... args)
                    {
                        return relics.template CreateChildWith<T>(structure, std::forward<InitializeArgs>(args)...);
                    },
                    args);
            };
        }

        template<class... InitializeArgs>
        explicit CreateChildWith(const Handle& parent, const std::string& structureName, InitializeArgs&& ... initializeArgs)
        {
            function =
                [parent, structureName, args = std::make_tuple(std::forward<InitializeArgs>(initializeArgs)...)](ReliquaryRelics& relics) mutable
            {
                return std::apply(
                    [parent, structureName, &relics](auto&& ... args)
                    {
                        return relics.template CreateChildWith<T>(structureName, std::forward<InitializeArgs>(args)...);
                    },
                    args);
            };
        }

        static_assert(is_relic_v<T>, "CreateChildWith can only be used with relics.");
    };

    template<class T>
    struct Traits<CreateChildWith<T>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "CreateChildWith";
        using Return = RelicIndex<T>;
    };
}