#include "OpenRelic.h"

#include "Reliquary.h"

namespace Arca
{
    OpenRelic::operator bool() const
    {
        if (!owner)
            return false;

        return owner->Contains<OpenRelic>(id);
    }

    std::optional<Handle> OpenRelic::Parent() const
    {
        return owner->ParentOf(AsHandle(*this));
    }

    RelicID OpenRelic::ID() const
    {
        return id;
    }

    Reliquary& OpenRelic::Owner() const
    {
        return *owner;
    }
}