#pragma once

#include <type_traits>

namespace Arca
{
    class OpenTypedRelic;
    class OpenRelic;

    template <class T, class Enable = void>
    struct is_open : std::false_type
    {};

    template <class T>
    struct is_open<
        T,
        std::enable_if_t<(std::is_base_of_v<OpenTypedRelic, T> || std::is_same_v<OpenRelic, T>)>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_open_v = is_open<T>::value;
}