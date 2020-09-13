#include "OpenRelic.h"

#include "Reliquary.h"

namespace Arca
{
    OpenRelic::OpenRelic(RelicInit init) :
        id(init.id), owner(&init.owner)
    {}

    std::optional<Handle> OpenRelic::Parent() const
    {
        return owner->ParentOf(ID());
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