#pragma once

#include "RelicID.h"
#include "RelicTypeDescription.h"

namespace Arca
{
    class Reliquary;

    struct RelicExtension
    {
        const RelicID id;
        const RelicTypeDescription typeDescription;
        const Reliquary& owner;
    };
}