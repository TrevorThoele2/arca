#include "Curator.h"

#include "Reliquary.h"

namespace Arca
{
    Curator::Curator(Init init) : owner(&init.owner)
    {}

    MutablePointer Curator::MutablePointer()
    {
        return Arca::MutablePointer(Owner().relics, Owner().shards);
    }

    Reliquary& Curator::Owner()
    {
        return *owner;
    }

    const Reliquary& Curator::Owner() const
    {
        return *owner;
    }
}