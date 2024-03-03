#pragma once

#include "Signal.h"

#include "Index.h"
#include "IsMatrix.h"

namespace Arca
{
    template<class T>
    struct MatrixDissolved
    {
        Index<T> index;

        static_assert(is_matrix_v<T>, "MatrixFormed must be used for matrices.");
    };

    template<class T>
    struct Traits<MatrixDissolved<T>>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static const inline TypeName typeName = "Arca::MatrixDissolved<" + Traits<T>::typeName + ">";
    };
}