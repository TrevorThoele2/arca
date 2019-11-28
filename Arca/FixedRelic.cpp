#include "FixedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    FixedRelic::operator RelicHandle() const
    {
        return RelicHandle(ID(), Owner());
    }

    void FixedRelic::ParentTo(const RelicHandle& parent) const
    {
        owner->ParentRelicTo(parent, *this);
    }

    std::optional<RelicHandle> FixedRelic::Parent() const
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