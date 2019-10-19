#pragma once

namespace Arca
{
    template<class Relic>
    struct BeforeRelicDestroyed
    {
        using RelicT = Relic;
        Ref<RelicT> relic;
    };
}