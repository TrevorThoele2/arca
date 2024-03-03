#pragma once

#include "SignalTraits.h"

namespace Arca
{
    template<class T>
    struct Destroying
    {
        using Type = T;
        Type& object;
    };

    template<class T>
    struct Traits<Destroying<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
    };
}