#pragma once

#include <type_traits>

namespace Arca
{
    class ClosedTypedRelic;
    class ClosedRelic;

    template <class T, class Enable = void>
    struct is_closed : std::false_type
    {};

    template <class T>
    struct is_closed<
        T,
        std::enable_if_t<(std::is_base_of_v<ClosedTypedRelic, T> || std::is_same_v<ClosedRelic, T>)>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_closed_v = is_closed<T>::value;
}