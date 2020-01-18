#pragma once

#include "Traits.h"
#include "ObjectType.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct is_matrix : std::false_type
    {};

    template <class T>
    struct is_matrix<T, std::enable_if_t<Traits<T>::objectType == ObjectType::Matrix>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_matrix_v = is_matrix<T>::value;
}