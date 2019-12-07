#pragma once

#include "ClosedTypedRelic.h"
#include "Reliquary.h"
#include "ExtractShards.h"

namespace Arca
{
    template<class Derived, class... AllShards>
    class ClosedTypedRelicAutomation : public ClosedTypedRelic
    {
    public:
        using Shards = ShardList<AllShards...>;
    protected:
        ClosedTypedRelicAutomation() = default;
    protected:
        [[nodiscard]] auto ExtractShards() const
        {
            return Arca::ExtractShards<Shards>(ID(), Owner());
        }
    protected:
        [[nodiscard]] bool ReliquaryContainsSelf() const override final
        {
            return ReliquaryContainsSelfImpl<Derived>();
        }

        [[nodiscard]] Arca::TypeHandle TypeHandle() const override
        {
            return TypeHandleFor<Derived>();
        }
    private:
        template<class U, std::enable_if_t<!is_global_relic_v<U>, int> = 0>
        [[nodiscard]] bool ReliquaryContainsSelfImpl() const
        {
            return Owner().Contains<Derived>(ID());
        }

        template<class U, std::enable_if_t<is_global_relic_v<U>, int> = 0>
        [[nodiscard]] bool ReliquaryContainsSelfImpl() const
        {
            return Owner().Contains<Derived>();
        }
    };
}