#pragma once

#include "RelicInit.h"

namespace Arca
{
    template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int>>
    Index<ShardT> RelicInit::Create(ConstructorArgs&& ... constructorArgs) const
    {
        return shards->CreateFromInternal<ShardT>(id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    [[nodiscard]] Index<ShardT> RelicInit::Find() const
    {
        return owner.Find<ShardT>(id);
    }
}