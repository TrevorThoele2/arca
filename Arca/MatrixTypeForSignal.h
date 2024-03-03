#pragma once

#include "IsMatrixSignal.h"

namespace Arca
{
    template<class T>
    struct MatrixTypeForSignal;

    template<class T>
    struct MatrixTypeForSignal<MatrixFormed<T>>
    {
        using Type = T;
    };

    template<class T>
    struct MatrixTypeForSignal<MatrixDissolved<T>>
    {
        using Type = T;
    };
}