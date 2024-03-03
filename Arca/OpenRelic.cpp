#include "OpenRelic.h"

#include "Reliquary.h"

namespace Arca
{
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

    OpenRelic::OpenRelic(RelicInit init) :
        id(init.id), owner(&init.owner)
    {}
}