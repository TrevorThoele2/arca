#pragma once

#include "HasLocalityTrait.h"
#include "Locality.h"

namespace Arca
{
    template<class T, std::enable_if_t<has_locality_trait_v<T>, int> = 0>
    constexpr Locality LocalityFor()
    {
        return Traits<T>::locality;
    }

    template<class T, std::enable_if_t<!has_locality_trait_v<T>, int> = 0>
    constexpr Locality LocalityFor()
    {
        return Locality::Local;
    }
}