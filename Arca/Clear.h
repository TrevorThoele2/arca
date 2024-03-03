#pragma once

#include "Command.h"

#include "IsRelic.h"
#include "TypeFor.h"

namespace Arca
{
    class ReliquaryRelics;

    struct Clear
    {
        Type type;

        explicit Clear(Type type) : type(std::move(type))
        {}

        template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
        explicit Clear(Chroma::TypeIdentity<T>) : type(TypeFor<T>())
        {}
    };

    template<>
    struct Traits<Clear>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Arca::Clear";
    };
}