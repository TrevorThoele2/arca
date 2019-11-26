#pragma once

#include "SignalTraits.h"

namespace Arca
{
    template<class T>
    struct Created
    {
        using Type = T;
        Type& object;
    };

    template<class T>
    struct Traits<Created<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
    };
}