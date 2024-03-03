#include "OwnedRelic.h"

namespace Arca
{
    OwnedRelic::OwnedRelic(Arca::Reliquary& owner) : owner(&owner)
    {}

    OwnedRelic::OwnedRelic(const ::Inscription::BinaryTableData<OwnedRelic>& data)
    {}

    Reliquary& OwnedRelic::Owner()
    {
        return *owner;
    }

    const Reliquary& OwnedRelic::Owner() const
    {
        return *owner;
    }
}