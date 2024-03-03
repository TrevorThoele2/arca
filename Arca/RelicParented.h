#pragma once

#include "SignalTraits.h"
#include "RelicHandle.h"

namespace Arca
{
    struct RelicParented
    {
        RelicHandle parent;
        RelicHandle child;
    };

    template<>
    struct Traits<RelicParented>
    {
        static const ObjectType objectType = ObjectType::Signal;
    };
}
