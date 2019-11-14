#pragma once

namespace Arca
{
    template<class Relic>
    struct RelicCreated
    {
        using RelicT = Relic;
        RelicT& relic;
    };
}