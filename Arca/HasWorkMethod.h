#pragma once

#include <type_traits>
#include "Curator.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct has_empty_work_method : std::false_type
    {};

    template <class T>
    struct has_empty_work_method<T, std::void_t<decltype(std::declval<T>().Work())>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_empty_work_method_v = has_empty_work_method<T>::value;

    template <class T, class Enable = void>
    struct has_stage_work_method : std::false_type
    {};

    template <class T>
    struct has_stage_work_method<T, std::void_t<decltype(std::declval<T>().Work(std::declval<Curator::Stage&>()))>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_stage_work_method_v = has_stage_work_method<T>::value;
}