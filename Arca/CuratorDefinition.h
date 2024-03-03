#pragma once

#include "Curator.h"
#include "Reliquary.h"

namespace Arca
{
    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicT* Curator::Data(RelicID id)
    {
        return Owner().template FindStorage<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicT* Curator::Data(RelicIndex<RelicT> index)
    {
        return Data<RelicT>(index.ID());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicT* Curator::Data(const RelicT& relic)
    {
        return Data<RelicT>(relic.ID());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicT* Curator::Data()
    {
        return Owner().template FindGlobalStorage<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicT* Curator::Data(GlobalIndex<RelicT>)
    {
        return Data<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicT* Curator::Data(const RelicT&)
    {
        return Data<RelicT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Curator::Data(RelicID id)
    {
        return Owner().template FindStorage<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Curator::Data(ShardIndex<ShardT> index)
    {
        return Data<ShardT>(index.ID());
    }
}