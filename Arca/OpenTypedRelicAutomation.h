#pragma once

#include "OpenTypedRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class Derived>
    class OpenTypedRelicAutomation : public OpenTypedRelic
    {
    protected:
        OpenTypedRelicAutomation() = default;
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
        template<class U, std::enable_if_t<!(is_global_v<U> && is_relic_v<U>), int> = 0>
        [[nodiscard]] bool ReliquaryContainsSelfImpl() const
        {
            return Owner().Contains<Derived>(ID());
        }

        template<class U, std::enable_if_t<is_global_v<U> && is_relic_v<U>, int> = 0>
        [[nodiscard]] bool ReliquaryContainsSelfImpl() const
        {
            return Owner().Contains<Derived>();
        }
    private:
        friend class ReliquaryOrigin;
        friend class ReliquaryRelics;
    };
}