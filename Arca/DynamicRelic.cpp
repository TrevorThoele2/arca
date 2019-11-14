#include "DynamicRelic.h"

#include "Reliquary.h"

namespace Arca
{
    Reliquary& DynamicRelic::Owner()
    {
        return *owner;
    }

    const Reliquary& DynamicRelic::Owner() const
    {
        return *owner;
    }

    RelicID DynamicRelic::ID() const
    {
        return id;
    }

    RelicDynamism DynamicRelic::Dynamism() const
    {
        return dynamism;
    }

    DynamicRelic::DynamicRelic(RelicID id, RelicDynamism dynamism, Reliquary& owner)
        : owner(&owner), id(id), dynamism(dynamism)
    {}
}

namespace Inscription
{
    void Scribe<Arca::DynamicRelic, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.owner);
        archive(object.id);
        archive(object.dynamism);
    }
}