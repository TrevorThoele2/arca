#include "TypedRelic.h"

namespace Arca
{
    Reliquary& TypedRelic::Owner()
    {
        return *owner;
    }

    const Reliquary& TypedRelic::Owner() const
    {
        return *owner;
    }

    RelicID TypedRelic::ID() const
    {
        return id;
    }

    const RelicStructure TypedRelic::structure;

    TypedRelic::TypedRelic(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    TypedRelic::TypedRelic(const ::Inscription::BinaryTableData<TypedRelic>& data) : id(data.id), owner(data.owner)
    {}
}