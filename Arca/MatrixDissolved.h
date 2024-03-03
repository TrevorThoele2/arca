#pragma once

#include "Signal.h"

#include "MatrixPtr.h"
#include "IsMatrix.h"

namespace Arca
{
    template<class T>
    struct MatrixDissolved
    {
        MatrixPtr<T> ptr;

        static_assert(is_matrix_v<T>, "MatrixFormed must be used for matrices.");
    };

    template<class T>
    struct Traits<MatrixDissolved<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "MatrixDissolved<" + Traits<T>::typeName + ">";
    };
}