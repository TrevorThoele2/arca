#pragma once

#include "RelicHandle.h"
#include "Ptr.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    Ptr<T> Actualize(const RelicHandle& handle)
    {
        return handle.Owner().Find<T>(handle.ID());
    }
}