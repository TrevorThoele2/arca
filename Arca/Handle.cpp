#include "Handle.h"

#include "Reliquary.h"

namespace Arca
{
    Handle::Handle(RelicID id, Reliquary& owner, Arca::Type type, HandleObjectType objectType)
        : id(id), owner(&owner), type(std::move(type)), objectType(objectType)
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
        return HandleSlim(ID(), Type(), ObjectType());
    }

    Reliquary& Handle::Owner() const
    {
        return *owner;
    }

    RelicID Handle::ID() const
    {
        return id;
    }

    Type Handle::Type() const
    {
        return type;
    }

    HandleObjectType Handle::ObjectType() const
    {
        return objectType;
    }
}