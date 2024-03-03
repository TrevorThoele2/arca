#pragma once

#include <type_traits>

namespace Arca
{
    class Reliquary;

    template <class T, class Enable = void>
    struct has_handled_commands : std::false_type
    {};

    template <class T>
    struct has_handled_commands<T, std::void_t<typename T::HandledCommands>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_handled_commands_v = has_handled_commands<T>::value;
}
