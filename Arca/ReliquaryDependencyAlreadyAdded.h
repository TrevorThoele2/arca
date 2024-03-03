#pragma once

#include "Exception.h"

namespace Arca
{
    class Reliquary;

    class ReliquaryDependencyAlreadyAdded final : public Exception
    {
    public:
        ReliquaryDependencyAlreadyAdded(const std::string& name);
        ReliquaryDependencyAlreadyAdded(Reliquary& reliquary);
    };
}