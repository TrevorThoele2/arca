#pragma once

#include <type_traits>

namespace Arca
{
    class Reliquary;

    template <class T, class = void>
    struct has_required_shards_method : std::false_type
    {};

    template <class T>
    struct has_required_shards_method<T, std::void_t<typename T::Shards>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_required_shards_method_v = has_required_shards_method<T>::value;
}