#pragma once

#include "IsMatrix.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_matrix_index_v = is_matrix_v<T>;
}