#pragma once

#include "Exception.h"

namespace Arca
{
    class Reliquary;

    class CannotFindDependent final : public Exception
    {
    public:
        CannotFindDependent(int id);
        CannotFindDependent(Reliquary* reliquary);
    };
}