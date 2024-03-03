#pragma once

#include "Traits.h"
#include "Handle.h"

namespace Arca
{
    struct RelicParented
    {
        Handle parent;
        Handle child;
    };

    template<>
    struct Traits<RelicParented>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static TypeName TypeName() { return "Arca::RelicParented"; }
    };
}
