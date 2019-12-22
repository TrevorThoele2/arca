#pragma once

#include "RelicID.h"

namespace Arca
{
    class Reliquary;

    struct RelicInitialization
    {
        const RelicID id;
        Reliquary& owner;
        RelicInitialization(RelicID id, Reliquary& owner) : id(id), owner(owner)
        {}
    };
}