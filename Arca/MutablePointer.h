#pragma once

#include "IsRelic.h"
#include "IsLocal.h"
#include "IsGlobal.h"
#include "IsShard.h"

#include "RelicID.h"

#include "RelicIndex.h"
#include "ShardIndex.h"

namespace Arca
{
    class MutablePointer
    {
    public:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of(RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of(Index<RelicT> index);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of(const RelicT& relic);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of();
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of(Index<RelicT> index);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Of(const RelicT& relic);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* Of(RelicID id);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* Of(Index<ShardT> index);

        [[nodiscard]] Arca::Reliquary& Reliquary();
        [[nodiscard]] const Arca::Reliquary& Reliquary() const;
    private:
        explicit MutablePointer(Arca::Reliquary& reliquary);
        Arca::Reliquary* reliquary;
        friend class Curator;
    };
}