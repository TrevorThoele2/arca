#pragma once

#include "Signal.h"

#include "MatrixPtr.h"
#include "IsMatrix.h"

namespace Arca
{
    template<class T>
    struct MatrixFormed
    {
        MatrixPtr<T> ptr;

        static_assert(is_matrix_v<T>, "MatrixFormed must be used for matrices.");
    };

    template<class T>
    struct Traits<MatrixFormed<T>>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "MatrixFormed<" + Traits<T>::typeName + ">";
    };
}