#include "FixedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    FixedRelic::operator Handle() const
    {
        return Handle(ID(), Owner());
    }

    void FixedRelic::ParentTo(const Handle& parent) const
    {
        owner->ParentRelicTo(parent, *this);
    }

    std::optional<Handle> FixedRelic::Parent() const
    {
        return owner->ParentOf(*this);
    }

    RelicID FixedRelic::ID() const
    {
        return id;
    }

    Reliquary& FixedRelic::Owner() const
    {
        return *owner;
    }

    FixedRelic::FixedRelic(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}
}