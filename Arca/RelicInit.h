#pragma once

#include "RelicID.h"

namespace Arca
{
    class Reliquary;

    struct RelicInit
    {
        RelicID id;
        Reliquary& owner;
    };
}