#include "ClosedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    std::optional<Handle> ClosedRelic::Parent() const
    {
        return owner->ParentOf(AsHandle(*this));
    }

    RelicID ClosedRelic::ID() const
    {
        return id;
    }

    Reliquary& ClosedRelic::Owner() const
    {
        return *owner;
    }

    ClosedRelic::ClosedRelic(RelicInitialization initialization) :
        id(initialization.id), owner(&initialization.owner)
    {}
}