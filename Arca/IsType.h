#pragma once

#include "Type.h"

#include "RelicTraits.h"
#include "ShardTraits.h"
#include "CuratorTraits.h"

namespace Arca
{
    template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
    bool IsType(const TypeName& typeName)
    {
        return typeName == Traits<T>::typeName;
    }

    template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
    bool IsType(const TypeName& typeName)
    {
        return typeName == Traits<T>::typeName;
    }

    template<class T, std::enable_if_t<is_curator_v<T>, int> = 0>
    bool IsType(const TypeName& typeName)
    {
        return typeName == Traits<T>::typeName;
    }
}
