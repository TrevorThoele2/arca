#include "RelicHandle.h"

namespace Arca
{
    RelicHandle::RelicHandle(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    bool RelicHandle::operator==(const RelicHandle& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    bool RelicHandle::operator!=(const RelicHandle& arg) const
    {
        return !(*this == arg);
    }

    Reliquary& RelicHandle::Owner() const
    {
        return *owner;
    }

    RelicID RelicHandle::ID() const
    {
        return id;
    }
}