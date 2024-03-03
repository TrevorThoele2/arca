#pragma once

#include "Signal.h"

#include "Index.h"
#include "IsMatrix.h"

namespace Arca
{
    template<class T>
    struct MatrixFormed
    {
        Index<T> index;

        static_assert(is_matrix_v<T>, "MatrixFormed must be used for matrices.");
    };

    template<class T>
    struct Traits<MatrixFormed<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static TypeName TypeName() { return "Arca::MatrixFormed<" + Traits<T>::TypeName() + ">"; }
    };
}