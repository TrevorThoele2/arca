#pragma once

#include "Traits.h"
#include "Handle.h"
#include "LocalPtr.h"

namespace Arca
{
    struct Destroying
    {
        Handle handle;
    };

    template<class T>
    struct DestroyingKnown
    {
        LocalPtr<T> ptr;
    };

    template<>
    struct Traits<Destroying>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "Destroying";
    };

    template<class T>
    struct Traits<DestroyingKnown<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "DestroyingKnown";
    };
}
