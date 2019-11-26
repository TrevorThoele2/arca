#include "RelicStructure.h"

namespace Arca
{
    RelicStructureEntry::RelicStructureEntry(TypeHandle typeHandle, bool isConst) :
        typeHandle(std::move(typeHandle)), isConst(isConst)
    {}
}