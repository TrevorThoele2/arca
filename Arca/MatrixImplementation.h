#pragma once

#include <unordered_set>
#include "TypeFor.h"

namespace Arca
{
    template<class T>
    class MatrixImplementation;

    template<class T>
    struct MatrixContains
    {
        static bool Contains(Type type)
        {
            return TypeFor<T>() == type;
        }
    };
}