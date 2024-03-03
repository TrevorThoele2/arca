#include "HandleSlim.h"

namespace Arca
{
    HandleSlim::HandleSlim(RelicID id, Arca::Type type, HandleObjectType objectType)
        : id(id), type(std::move(type)), objectType(objectType)
    {}

    bool HandleSlim::operator==(const HandleSlim& arg) const
    {
        return id == arg.id && type == arg.type;
    }

    bool HandleSlim::operator!=(const HandleSlim& arg) const
    {
        return !(*this == arg);
    }

    RelicID HandleSlim::ID() const
    {
        return id;
    }

    Type HandleSlim::Type() const
    {
        return type;
    }

    HandleObjectType HandleSlim::ObjectType()
    {
        return objectType;
    }
}