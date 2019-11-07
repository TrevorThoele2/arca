#include "Relic.h"

#include "Reliquary.h"

namespace Arca
{
    Reliquary& Relic::Owner()
    {
        return *owner;
    }

    const Reliquary& Relic::Owner() const
    {
        return *owner;
    }

    RelicID Relic::ID() const
    {
        return id;
    }

    RelicDynamism Relic::Dynamism() const
    {
        return dynamism;
    }

    Relic::Relic(RelicID id, RelicDynamism dynamism, Reliquary& owner)
        : owner(&owner), id(id), dynamism(dynamism)
    {}
}

namespace Inscription
{
    void Scribe<Arca::Relic, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.owner);
        archive(object.id);
        archive(object.dynamism);
    }
}