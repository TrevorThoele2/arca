#include "Curator.h"

#include "Reliquary.h"

namespace Arca
{
    void Curator::Stage::Abort()
    {
        isAborted = true;
    }

    bool Curator::Stage::IsAborted() const
    {
        return isAborted;
    }

    Curator::Curator(Reliquary& owner) : owner(&owner)
    {}

    Reliquary& Curator::Owner()
    {
        return *owner;
    }

    const Reliquary& Curator::Owner() const
    {
        return *owner;
    }
}