#include "FixedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    FixedRelic::operator Handle() const
    {
        return Handle(ID(), Owner());
    }

    FixedRelic::operator bool() const
    {
        if (!owner)
            return false;

        return owner->Contains<FixedRelic>(id);
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

namespace Inscription
{
    void Scribe<Arca::FixedRelic, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.id);
        archive(object.owner);
    }
}