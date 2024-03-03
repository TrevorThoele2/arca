#pragma once

#include <type_traits>
#include "RelicInit.h"
#include "SerializationConstructorMarker.h"

namespace Arca
{
    template<class T, class Enable = void>
    struct has_relic_serialization_constructor : std::false_type
    {};

    template<class T>
    struct has_relic_serialization_constructor<
        T,
        std::void_t<decltype(void(T{ std::declval<RelicInit>(), std::declval<Serialization>() }))>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_relic_serialization_constructor_v = has_relic_serialization_constructor<T>::value;
}