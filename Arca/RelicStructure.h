#pragma once

#include <vector>
#include "TypeHandle.h"

namespace Arca
{
    struct RelicStructureEntry
    {
        TypeHandle typeHandle;
        bool isConst;

        RelicStructureEntry(TypeHandle typeHandle, bool isConst = false);
    };

    using RelicStructure = std::vector<RelicStructureEntry>;
}