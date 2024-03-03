#pragma once

#include <type_traits>

namespace Arca
{
    template<class T>
    struct has_shard_default_constructor : std::is_default_constructible<T>
    {};

    template<class T>
    static constexpr bool has_shard_default_constructor_v = has_shard_default_constructor<T>::value;
}