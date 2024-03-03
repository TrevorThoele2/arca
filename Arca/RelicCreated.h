#pragma once

namespace Arca
{
    template<class RelicT>
    struct RelicCreated
    {
        using Relic = RelicT;
        Relic& relic;
    };
}