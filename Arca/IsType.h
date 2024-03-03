#pragma once

#include "TypeHandle.h"

#include "RelicTraits.h"
#include "ShardTraits.h"
#include "CuratorTraits.h"

#include "RelicMetadata.h"

namespace Arca
{
    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    bool IsType(const TypeHandle& typeHandle)
    {
        return typeHandle == Traits<T>::typeHandle;
    }

    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    bool IsType(const TypeHandle& typeHandle)
    {
        return typeHandle == Traits<T>::typeHandle;
    }

    template<class T, std::enable_if_t<is_curator_v<T>, int> = 0>
    bool IsType(const TypeHandle& typeHandle)
    {
        return typeHandle == Traits<T>::typeHandle;
    }

    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    bool IsType(const RelicMetadata& metadata)
    {
        if (!metadata.typeHandle.has_value())
            return false;

        return IsType<T>(metadata.typeHandle);
    }

    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    bool IsType(const RelicMetadata& metadata)
    {
        if (!metadata.typeHandle.has_value())
            return false;

        return IsType<T>(metadata.typeHandle);
    }

    template<class T, std::enable_if_t<is_curator_v<T>, int> = 0>
    bool IsType(const RelicMetadata& metadata)
    {
        if (!metadata.typeHandle.has_value())
            return false;

        return IsType<T>(metadata.typeHandle);
    }
}
