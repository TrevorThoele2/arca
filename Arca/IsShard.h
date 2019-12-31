#pragma once

#include <type_traits>

#include "Traits.h"

namespace Arca
{
    template <class T, class Enable = void>
    struct is_shard : std::false_type
    {};

    template <class T>
    struct is_shard<T, std::enable_if_t<Traits<std::decay_t<T>>::objectType == ObjectType::Shard>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_shard_v = is_shard<T>::value;
}