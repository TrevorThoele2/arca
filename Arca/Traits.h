#pragma once

#include "TypeHandle.h"
#include "ObjectType.h"

namespace Arca
{
    template<class T>
    struct Traits;

    template<class T>
    TypeHandle TypeHandleFor()
    {
        return Traits<std::decay_t<T>>::typeHandle;
    }
}