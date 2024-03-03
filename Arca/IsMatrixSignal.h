#pragma once

#include <type_traits>

#include "MatrixFormed.h"
#include "MatrixDissolved.h"

namespace Arca
{
    template <class T>
    struct is_matrix_signal : std::false_type
    {};

    template <class T>
    struct is_matrix_signal<MatrixFormed<T>> : std::true_type
    {};

    template <class T>
    struct is_matrix_signal<MatrixDissolved<T>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_matrix_signal_v = is_matrix_signal<T>::value;
}