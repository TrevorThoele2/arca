#pragma once

#include "MutablePointer.h"
#include "Reliquary.h"

namespace Arca
{
    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicT* MutablePointer::Of(RelicID id)
    {
        return reliquary->template FindStorage<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicT* MutablePointer::Of(Index<RelicT> index)
    {
        return Of<RelicT>(index.ID());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicT* MutablePointer::Of(const RelicT& relic)
    {
        return Of<RelicT>(relic.ID());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicT* MutablePointer::Of()
    {
        return reliquary->template FindGlobalStorage<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicT* MutablePointer::Of(Index<RelicT>)
    {
        return Of<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicT* MutablePointer::Of(const RelicT&)
    {
        return Of<RelicT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* MutablePointer::Of(RelicID id)
    {
        return reliquary->template FindStorage<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* MutablePointer::Of(Index<ShardT> index)
    {
        return Of<ShardT>(index.ID());
    }
}