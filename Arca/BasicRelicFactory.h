#pragma once

#include <optional>

#include "RelicFactoryBase.h"

namespace Arca
{
    template<class RelicT>
    class BasicRelicFactory : public RelicFactoryBase
    {
    public:
        using Relic = RelicT;
        using Created = std::optional<Relic>;
    public:
        using RelicFactoryBase::RelicFactoryBase;

        template<class... RelicArgs>
        Created CreateRelic(RelicArgs&& ... relicArgs);
    };

    template<class RelicT>
    template<class... RelicArgs>
    auto BasicRelicFactory<RelicT>::CreateRelic(RelicArgs&& ... relicArgs) -> Created
    {
        return RelicT{ std::forward<RelicArgs>(relicArgs)... };
    }
}