#pragma once

#include "Handle.h"
#include "Ptr.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    Ptr<T> Actualize(const Handle& handle)
    {
        return handle.Owner().Find<T>(handle.ID());
    }
}