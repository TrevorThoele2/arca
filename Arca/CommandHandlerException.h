#pragma once

#include "Exception.h"
#include "Type.h"

namespace Arca
{
    class CommandNotRegisteredInCurator final : public Exception
    {
    public:
        CommandNotRegisteredInCurator(const TypeName& curatorType, const TypeName& commandType);
    };
}