#pragma once

#include "RelicMetadata.h"

namespace Arca
{
    struct RelicParented
    {
        RelicMetadata parent;
        RelicMetadata child;
    };
}
