#pragma once

#include "IsMatrix.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_matrix_ptr_v = is_matrix_v<T>;
}