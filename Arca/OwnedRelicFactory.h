#pragma once

#include <optional>

#include "RelicFactoryBase.h"

namespace Arca
{
    template<class RelicT>
    class OwnedRelicFactory : public RelicFactoryBase
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
    auto OwnedRelicFactory<RelicT>::CreateRelic(RelicArgs&& ... relicArgs) -> Created
    {
        return RelicT{ Owner(), std::forward<RelicArgs>(relicArgs)... };
    }
}