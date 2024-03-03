#pragma once

namespace Arca
{
    template<class RelicT>
    struct BeforeRelicDestroyed
    {
        using Relic = RelicT;
        Relic& relic;
    };
}