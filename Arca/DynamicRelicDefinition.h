#pragma once

#include "DynamicRelic.h"

namespace Arca
{
    template<class ShardT>
    ShardT* DynamicRelic::CreateShard()
    {
        return owner->CreateShard<ShardT>(id);
    }

    template<class ShardT>
    void DynamicRelic::DestroyShard()
    {
        owner->DestroyShard<ShardT>(id);
    }

    template<class ShardT>
    ShardT* DynamicRelic::FindShard()
    {
        return owner->FindShard<ShardT>(id);
    }

    template<class ShardT>
    const ShardT* DynamicRelic::FindShard() const
    {
        return owner->FindShard<ShardT>(id);
    }

    template<class ShardT>
    bool DynamicRelic::HasShard() const
    {
        return FindShard<ShardT>() != nullptr;
    }
}