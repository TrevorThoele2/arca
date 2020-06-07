#pragma once

#include "Exception.h"

namespace Arca
{
    class Reliquary;

    class CannotFindReliquaryDependency final : public Exception
    {
    public:
        CannotFindReliquaryDependency(int id);
        CannotFindReliquaryDependency(Reliquary* reliquary);
    };
}