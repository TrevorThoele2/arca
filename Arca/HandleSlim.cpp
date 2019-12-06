#include "HandleSlim.h"

namespace Arca
{
    HandleSlim::HandleSlim(RelicID id, TypeHandle type)
        : id(id), type(std::move(type))
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

    TypeHandle HandleSlim::Type() const
    {
        return type;
    }
}