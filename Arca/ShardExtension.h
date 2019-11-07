#pragma once

#include "ShardID.h"
#include "ShardTypeDescription.h"

namespace Arca
{
    class Reliquary;

    struct ShardExtension
    {
        const ShardID id;
        const ShardTypeDescription typeDescription;
        const Reliquary& owner;
    };
}