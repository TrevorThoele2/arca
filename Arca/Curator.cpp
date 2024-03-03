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

    Curator::Curator(Init init) : owner(&init.owner)
    {}

    MutablePointer Curator::MutablePointer()
    {
        return Arca::MutablePointer(Owner());
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