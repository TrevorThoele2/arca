#pragma once

#include "CuratorInit.h"
#include "HandledCommands.h"

#include "IsRelic.h"
#include "IsShard.h"
#include "RelicID.h"

#include "RelicIndex.h"
#include "ShardIndex.h"
#include "GlobalIndex.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class Curator
    {
    public:
        class Stage
        {
        public:
            Stage() = default;
            void Abort();
            [[nodiscard]] bool IsAborted() const;
        private:
            bool isAborted = false;
        };
    public:
        Curator(const Curator& arg) = delete;
        Curator(Curator&& arg) = delete;
        
        Curator& operator=(const Curator& arg) = delete;
        Curator& operator=(Curator&& arg) = delete;
    protected:
        using Init = CuratorInit;
        explicit Curator(Init init);
    protected:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Data(RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Data(RelicIndex<RelicT> index);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Data(const RelicT& relic);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Data();
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Data(GlobalIndex<RelicT> index);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicT* Data(const RelicT& relic);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* Data(RelicID id);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* Data(ShardIndex<ShardT> index);
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
    private:
        Reliquary* owner = nullptr;
    private:
        INSCRIPTION_ACCESS;
        friend class ReliquaryCurators;
    };
}
