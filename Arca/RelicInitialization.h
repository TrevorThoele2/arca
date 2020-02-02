#pragma once

#include "RelicID.h"

namespace Arca
{
    class Reliquary;

    struct RelicInitialization
    {
        RelicID id;
        Reliquary& owner;
    };
}