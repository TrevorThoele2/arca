#pragma once

#include "IsRelic.h"
#include "IsLocal.h"
#include "IsGlobal.h"
#include "IsShard.h"

#include "RelicID.h"

#include "RelicIndex.h"
#include "ShardIndex.h"

#include "ReliquaryRelics.h"
#include "ReliquaryShards.h"

namespace Arca
{
    class MutablePointer
    {
    public:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of(RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of(Index<RelicT> index);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of();
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of(Index<RelicT> index);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* Of(RelicID id);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* Of(Index<ShardT> index);
    private:
        explicit MutablePointer(ReliquaryRelics& relics, ReliquaryShards& shards);
        ReliquaryRelics* relics;
        ReliquaryShards* shards;
        friend class Curator;
    };

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_local_v<RelicT>, int>>
    RelicT* MutablePointer::Of(RelicID id)
    {
        return relics->FindStorage<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_local_v<RelicT>, int>>
    RelicT* MutablePointer::Of(Index<RelicT> index)
    {
        return Of<RelicT>(index.ID());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_global_v<RelicT>, int>>
    RelicT* MutablePointer::Of()
    {
        return relics->FindGlobalStorage<RelicT>();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_global_v<RelicT>, int>>
    RelicT* MutablePointer::Of(Index<RelicT>)
    {
        return Of<RelicT>();
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* MutablePointer::Of(RelicID id)
    {
        return shards->FindStorage<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* MutablePointer::Of(Index<ShardT> index)
    {
        return Of<ShardT>(index.ID());
    }
}