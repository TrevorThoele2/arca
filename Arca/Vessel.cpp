#include "Vessel.h"

#include "Reliquary.h"

namespace Arca
{
    Reliquary& Vessel::Owner()
    {
        return *owner;
    }

    const Reliquary& Vessel::Owner() const
    {
        return *owner;
    }

    VesselID Vessel::ID() const
    {
        return id;
    }

    VesselDynamism Vessel::Dynamism() const
    {
        return dynamism;
    }

    Vessel::Vessel(VesselID id, VesselDynamism dynamism, Reliquary& owner)
        : owner(&owner), id(id), dynamism(dynamism)
    {}
}

namespace Inscription
{
    void Scribe<Arca::Vessel, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.owner);
        archive(object.id);
        archive(object.dynamism);
    }
}