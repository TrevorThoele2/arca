#pragma once

#include "Curator.h"
#include "Reliquary.h"

namespace Arca
{
    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicT* Curator::MutablePointer(RelicID id)
    {
        return Owner().template FindStorage<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicT* Curator::MutablePointer(RelicIndex<RelicT> index)
    {
        return MutablePointer<RelicT>(index.ID());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicT* Curator::MutablePointer(const RelicT& relic)
    {
        return MutablePointer<RelicT>(relic.ID());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicT* Curator::MutablePointer()
    {
        return Owner().template FindGlobalStorage<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicT* Curator::MutablePointer(GlobalIndex<RelicT>)
    {
        return MutablePointer<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicT* Curator::MutablePointer(const RelicT&)
    {
        return MutablePointer<RelicT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Curator::MutablePointer(RelicID id)
    {
        return Owner().template FindStorage<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Curator::MutablePointer(ShardIndex<ShardT> index)
    {
        return MutablePointer<ShardT>(index.ID());
    }
}