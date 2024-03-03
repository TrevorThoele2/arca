#pragma once

#include "Relic.h"

namespace Arca
{
    template<class ShardT>
    ShardT* Relic::CreateShard()
    {
        return owner->CreateShard<ShardT>(id);
    }

    template<class ShardT>
    void Relic::DestroyShard()
    {
        owner->DestroyShard<ShardT>(id);
    }

    template<class ShardT>
    ShardT* Relic::FindShard()
    {
        return owner->FindShard<ShardT>(id);
    }

    template<class ShardT>
    const ShardT* Relic::FindShard() const
    {
        return owner->FindShard<ShardT>(id);
    }

    template<class ShardT>
    bool Relic::HasShard() const
    {
        return FindShard<ShardT>() != nullptr;
    }
}