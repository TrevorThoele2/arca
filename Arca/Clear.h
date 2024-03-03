#pragma once

#include "Command.h"

#include "IsRelic.h"
#include "TypeFor.h"

namespace Arca
{
    class Reliquary;

    struct Clear
    {
        Type type;

        explicit Clear(Type type);
        template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
        explicit Clear(Chroma::TypeIdentity<T>);

        void Do(Reliquary& reliquary) const;
    };

    template<class T, std::enable_if_t<is_relic_v<T>, int>>
    Clear::Clear(Chroma::TypeIdentity<T>) : type(TypeFor<T>())
    {}

    template<>
    struct Traits<Clear>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static constexpr TypeName TypeName() { return "Arca::Clear"; }
        static constexpr bool selfContained = true;
    };
}