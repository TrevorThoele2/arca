#pragma once

#include "SignalTraits.h"

namespace Arca
{
    struct Destroying
    {
        Handle handle;
    };

    template<>
    struct Traits<Destroying>
    {
        static const ObjectType objectType = ObjectType::Signal;
    };
}
