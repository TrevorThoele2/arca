#pragma once

#include "Traits.h"
#include "Handle.h"
#include "LocalPtr.h"
#include "TypeFor.h"

namespace Arca
{
    struct Created
    {
        Handle handle;
    };

    template<class T>
    struct CreatedKnown
    {
        typename PtrTypeFor<T>::Type ptr;
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
        static inline const TypeName typeName = "CreatedKnown<" + Traits<std::decay_t<T>>::typeName + ">";
    };
}
