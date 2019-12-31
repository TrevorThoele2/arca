#pragma once

#include "Traits.h"
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
        static const TypeName typeName;
    };
}
