#include "SlimHandle.h"

#include "Reliquary.h"

namespace Arca
{
    SlimHandle::SlimHandle(RelicID id, Arca::Type type, HandleObjectType objectType) :
        id(id), type(std::move(type)), objectType(objectType)
    {}

    bool SlimHandle::operator==(const SlimHandle& arg) const
    {
        return id == arg.id && type == arg.type;
    }

    bool SlimHandle::operator!=(const SlimHandle& arg) const
    {
        return !(*this == arg);
    }

    RelicID SlimHandle::ID() const
    {
        return id;
    }

    Type SlimHandle::Type() const
    {
        return type;
    }

    HandleObjectType SlimHandle::ObjectType()
    {
        return objectType;
    }
}