#pragma once

#include "Exception.h"

namespace Arca
{
    class CuratorDependencyNotFound final : public Exception
    {
    public:
        CuratorDependencyNotFound();
    };
}