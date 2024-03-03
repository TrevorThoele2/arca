#include "DynamicRelic.h"

#include "Reliquary.h"

namespace Arca
{
    DynamicRelic::operator Handle() const
    {
        return Handle(id, *owner);
    }

    void DynamicRelic::ParentTo(const Handle& parent) const
    {
        owner->ParentRelicTo(parent, *this);
    }

    std::optional<Handle> DynamicRelic::Parent() const
    {
        return owner->ParentOf(*this);
    }

    RelicID DynamicRelic::ID() const
    {
        return id;
    }

    Reliquary& DynamicRelic::Owner() const
    {
        return *owner;
    }

    DynamicRelic::DynamicRelic(RelicID id, Reliquary& owner)
        : id(id), owner(&owner)
    {}
}

namespace Inscription
{
    void Scribe<Arca::DynamicRelic, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.id);
        archive(object.owner);
    }
}