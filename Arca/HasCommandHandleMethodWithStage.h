#pragma once

#include <type_traits>
#include "CuratorStage.h"

namespace Arca
{
    template <class T, class Command, class Enable = void>
    struct has_command_handle_method_with_stage : std::false_type
    {};

    template <class T, class Command>
    struct has_command_handle_method_with_stage<
        T,
        Command,
        std::void_t<decltype(std::declval<T>().Handle(std::declval<const Command&>(), std::declval<CuratorStage&>()))>> : std::true_type
    {};

    template<class T, class Command>
    static constexpr bool has_command_handle_method_with_stage_v =
        has_command_handle_method_with_stage<T, Command>::value;
}