#pragma once

#include "SignalTraits.h"
#include "Handle.h"

namespace Arca
{
    struct Created
    {
        Handle handle;
    };

    template<>
    struct Traits<Created>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeHandleName typeName;
    };
}
