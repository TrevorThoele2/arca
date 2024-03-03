#include "Handle.h"

namespace Arca
{
    Handle::Handle(RelicID id, Reliquary& owner, TypeHandle type)
        : id(id), owner(&owner), type(std::move(type))
    {}

    bool Handle::operator==(const Handle& arg) const
    {
        return id == arg.id && owner == arg.owner && type == arg.type;
    }

    bool Handle::operator!=(const Handle& arg) const
    {
        return !(*this == arg);
    }

    Handle::operator HandleSlim() const
    {
        return HandleSlim(ID(), Type());
    }

    Reliquary& Handle::Owner() const
    {
        return *owner;
    }

    RelicID Handle::ID() const
    {
        return id;
    }

    TypeHandle Handle::Type() const
    {
        return type;
    }
}