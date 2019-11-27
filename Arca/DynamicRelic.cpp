#include "DynamicRelic.h"

#include "Reliquary.h"

namespace Arca
{
    DynamicRelic::operator RelicHandle() const
    {
        return RelicHandle(id, *owner);
    }

    void DynamicRelic::ParentTo(const RelicHandle& parent) const
    {
        owner->ParentRelic(parent, *this);
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