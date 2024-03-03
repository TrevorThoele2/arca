#pragma once

#include <type_traits>

namespace Arca
{
    template <class T, class Enable = void>
    struct has_shards : std::false_type
    {};

    template <class T>
    struct has_shards<T, std::void_t<typename T::Shards>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_shards_v = has_shards<T>::value;
}