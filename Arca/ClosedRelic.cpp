#include "ClosedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    ClosedRelic::ClosedRelic(RelicInit init) :
        id(init.id), owner(&init.owner)
    {}

    std::optional<Handle> ClosedRelic::Parent() const
    {
        return owner->ParentOf(ID());
    }

    RelicID ClosedRelic::ID() const
    {
        return id;
    }

    Reliquary& ClosedRelic::Owner() const
    {
        return *owner;
    }
}