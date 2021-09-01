#include "Handle.h"

#include "Reliquary.h"

namespace Arca
{
    bool Handle::operator==(const Handle& arg) const
    {
        return id == arg.id && type == arg.type;
    }

    bool Handle::operator!=(const Handle& arg) const
    {
        return !(*this == arg);
    }
}