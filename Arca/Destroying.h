#pragma once

#include "SignalTraits.h"
#include "Ptr.h"

namespace Arca
{
    template<class T>
    struct Destroying
    {
        using Type = T;
        Ptr<T> object;
    };

    template<class T>
    struct Traits<Destroying<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
    };

    struct DestroyingRelic
    {
        RelicHandle handle;
    };

    template<>
    struct Traits<DestroyingRelic>
    {
        static const ObjectType objectType = ObjectType::Signal;
    };
}