#pragma once

#include "Handle.h"
#include "Index.h"

namespace Arca
{
    template<class T>
    auto Actualize(const Handle& handle)
    {
        if (handle.Type() != TypeFor<T>())
            return typename ReferenceTypeFor<T>::Type{};

        return ToReference<T>(handle.ID(), handle.Owner());
    }
}