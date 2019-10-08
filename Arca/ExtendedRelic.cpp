#include "ExtendedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    Reliquary& AnyExtendedRelic::Owner()
    {
        return *owner;
    }

    const Reliquary& AnyExtendedRelic::Owner() const
    {
        return *owner;
    }

    RelicExtension AnyExtendedRelic::Extension() const
    {
        return RelicExtension{ id, TypeDescription(), *owner };
    }

    AnyExtendedRelic::~AnyExtendedRelic() = default;

    AnyExtendedRelic::AnyExtendedRelic(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}
}