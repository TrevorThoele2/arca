#pragma once

#include "SignalTraits.h"
#include "Ptr.h"

namespace Arca
{
    template<class T>
    struct Created
    {
        using Type = T;
        Ptr<T> object;
    };

    template<class T>
    struct Traits<Created<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
    };
}