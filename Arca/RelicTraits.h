#pragma once

#include "Traits.h"

#include "Locality.h"
#include "Openness.h"
#include "ObjectType.h"
#include "Handle.h"

#include <Chroma/VariadicTemplate.h>

namespace Arca
{
    class OpenTypedRelic;
    class OpenRelic;
    class ClosedTypedRelic;
    class ClosedRelic;

    template <class T, class = void>
    struct is_relic : std::false_type
    {};

    template <class T>
    struct is_relic<T, std::enable_if_t<Traits<std::decay_t<T>>::objectType == ObjectType::Relic>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_relic_v = is_relic<T>::value;

    template <class T, class = void>
    struct is_open_relic : std::false_type
    {};

    template <class T>
    struct is_open_relic<
        T,
        std::enable_if_t<
            is_relic_v<T> && (std::is_base_of_v<OpenTypedRelic, T> || std::is_same_v<OpenRelic, T>)>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_open_relic_v = is_open_relic<T>::value;

    template <class T, class = void>
    struct is_closed_relic : std::false_type
    {};

    template <class T>
    struct is_closed_relic<
        T,
        std::enable_if_t<
            is_relic_v<T> && (std::is_base_of_v<ClosedTypedRelic, T> || std::is_same_v<ClosedRelic, T>)>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_closed_relic_v = is_closed_relic<T>::value;

    template<class T, std::enable_if_t<is_open_relic_v<T>, int> = 0>
    constexpr Openness OpennessFor()
    {
        return Openness::Open;
    }

    template<class T, std::enable_if_t<!is_open_relic_v<T>, int> = 0>
    constexpr Openness OpennessFor()
    {
        return Openness::Closed;
    }

    template<class T, class = void>
    struct has_locality : std::false_type
    {};

    template<class T>
    struct has_locality<T, std::void_t<decltype(Traits<T>::locality)>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_locality_v = has_locality<T>::value;

    template <class T, class = void>
    struct is_local_relic : std::false_type
    {};

    template <class T>
    struct is_local_relic<T, std::enable_if_t<is_relic_v<T> && !has_locality_v<T>>> : std::true_type
    {};

    template <class T>
    struct is_local_relic<T, std::enable_if_t<is_relic_v<T> && Traits<T>::locality == Locality::Local>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_local_relic_v = is_local_relic<T>::value;

    template <class T, class = void>
    struct is_global_relic : std::false_type
    {};

    template <class T>
    struct is_global_relic<T, std::enable_if_t<is_relic_v<T> && Traits<T>::locality == Locality::Global>> : std::true_type
    {};

    template<class T>
    static constexpr bool is_global_relic_v = is_global_relic<T>::value;

    template<class T, std::enable_if_t<has_locality_v<T>, int> = 0>
    constexpr Locality LocalityFor()
    {
        return Traits<T>::locality;
    }

    template<class T, std::enable_if_t<!has_locality_v<T>, int> = 0>
    constexpr Locality LocalityFor()
    {
        return Locality::Local;
    }

    template<class... Args>
    using ShardList = Chroma::VariadicTemplate<Args...>;

    template <class T, class = void>
    struct has_shards : std::false_type
    {};

    template <class T>
    struct has_shards<T, std::void_t<typename T::Shards>> : std::true_type
    {};

    template<class T>
    static constexpr bool has_shards_v = has_shards<T>::value;

    template<class T, bool>
    class DiscoverShards;

    template<class T>
    class DiscoverShards<T, true>
    {
    public:
        using Type = typename T::Shards;
    };

    template<class T>
    class DiscoverShards<T, false>
    {
    public:
        using Type = ShardList<>;
    };

    template<class T>
    using shards_for = DiscoverShards<T, has_shards_v<T>>;

    template<class T>
    using shards_for_t = typename shards_for<T>::Type;

    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    ObjectType ObjectTypeFor()
    {
        return ObjectType::Relic;
    }

    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    HandleObjectType HandleObjectTypeFor()
    {
        return HandleObjectType::Relic;
    }

    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    Handle AsHandle(const T& object)
    {
        return Handle(object.ID(), object.Owner(), TypeFor<T>(), HandleObjectType::Relic);
    }
}
