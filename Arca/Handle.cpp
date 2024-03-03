#include "Handle.h"

namespace Arca
{
    Handle::Handle(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    bool Handle::operator==(const Handle& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    bool Handle::operator!=(const Handle& arg) const
    {
        return !(*this == arg);
    }

    Reliquary& Handle::Owner() const
    {
        return *owner;
    }

    RelicID Handle::ID() const
    {
        return id;
    }
}