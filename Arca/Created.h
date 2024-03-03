#pragma once

#include "Traits.h"
#include "Handle.h"
#include "LocalPtr.h"

namespace Arca
{
    struct Created
    {
        Handle handle;
    };

    template<class T>
    struct CreatedKnown
    {
        LocalPtr<T> ptr;
    };

    template<>
    struct Traits<Created>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "Created";
    };

    template<class T>
    struct Traits<CreatedKnown<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "CreatedKnown";
    };
}
