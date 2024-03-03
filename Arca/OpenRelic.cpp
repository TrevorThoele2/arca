#include "OpenRelic.h"

#include "Reliquary.h"

namespace Arca
{
    OpenRelic::operator Handle() const
    {
        return Handle(id, *owner);
    }

    OpenRelic::operator bool() const
    {
        if (!owner)
            return false;

        return owner->Contains<OpenRelic>(id);
    }

    std::optional<Handle> OpenRelic::Parent() const
    {
        return owner->ParentOf(*this);
    }

    RelicID OpenRelic::ID() const
    {
        return id;
    }

    Reliquary& OpenRelic::Owner() const
    {
        return *owner;
    }

    OpenRelic::OpenRelic(RelicID id, Reliquary& owner)
        : id(id), owner(&owner)
    {}
}

namespace Inscription
{
    void Scribe<Arca::OpenRelic, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.id);
        archive(object.owner);
    }
}