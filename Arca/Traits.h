#pragma once

#include <type_traits>

#include "TypeHandle.h"
#include "ObjectType.h"

namespace Arca
{
    template<class T>
    struct Traits;

    template<class T, std::enable_if_t<!std::is_const_v<T>, int> = 0>
    TypeHandle TypeHandleFor()
    {
        return { Traits<std::decay_t<T>>::typeName, false };
    }

    template<class T, std::enable_if_t<std::is_const_v<T>, int> = 0>
    TypeHandle TypeHandleFor()
    {
        return { Traits<std::decay_t<T>>::typeName, true };
    }
}