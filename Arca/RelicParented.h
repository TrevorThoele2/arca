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
        static constexpr ObjectType objectType = ObjectType::Signal;
        static const inline TypeName typeName = "Arca::RelicParented";
    };
}
