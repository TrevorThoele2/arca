#pragma once

#include "SignalTraits.h"
#include "RelicMetadata.h"

namespace Arca
{
    struct RelicParented
    {
        RelicMetadata parent;
        RelicMetadata child;
    };

    template<>
    struct Traits<RelicParented>
    {
        static const ObjectType objectType = ObjectType::Signal;
    };
}
