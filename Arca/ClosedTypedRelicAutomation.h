#pragma once

#include "ClosedTypedRelic.h"
#include "Reliquary.h"
#include "TypeFor.h"

namespace Arca
{
    template<class Derived>
    class ClosedTypedRelicAutomation : public ClosedTypedRelic
    {
    protected:
        ClosedTypedRelicAutomation() = default;
        ClosedTypedRelicAutomation(const ClosedTypedRelicAutomation& arg) = default;
        ClosedTypedRelicAutomation(ClosedTypedRelicAutomation&& arg) noexcept = default;

        ClosedTypedRelicAutomation& operator=(const ClosedTypedRelicAutomation& arg) = default;
        ClosedTypedRelicAutomation& operator=(ClosedTypedRelicAutomation&& arg) noexcept = default;
    protected:
        template<class ShardT, class... InitializeArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ShardIndex<ShardT> Create(InitializeArgs&& ... initializeArgs) const
        {
            return Owner().template CreateFromInternal<ShardT>(ID(), std::forward<InitializeArgs>(initializeArgs)...);
        }

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ShardIndex<ShardT> Find() const
        {
            return ShardIndex<ShardT>(ID(), Owner());
        }
    protected:
        [[nodiscard]] bool ReliquaryContainsSelf() const override final
        {
            return ReliquaryContainsSelfImpl<Derived>();
        }

        [[nodiscard]] Arca::Type Type() const override
        {
            return TypeFor<Derived>();
        }
    private:
        template<class U, std::enable_if_t<!(is_relic_v<U> && is_global_v<U>), int> = 0>
        [[nodiscard]] bool ReliquaryContainsSelfImpl() const
        {
            return Owner().template Contains<Derived>(ID());
        }

        template<class U, std::enable_if_t<(is_relic_v<U> && is_global_v<U>), int> = 0>
        [[nodiscard]] bool ReliquaryContainsSelfImpl() const
        {
            return Owner().template Contains<Derived>();
        }
    private:
        friend class ReliquaryOrigin;
        friend class ReliquaryRelics;
    };
}