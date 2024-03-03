#pragma once

#include "Either.h"

namespace Arca
{
    template<class T>
    struct is_either : std::false_type
    {};

    template<class T>
    struct is_either<Either<T>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_either_v = is_either<T>::value;
}