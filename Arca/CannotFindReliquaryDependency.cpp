#include "CannotFindReliquaryDependency.h"

#include <Chroma/StringUtility.h>

namespace Arca
{
    CannotFindReliquaryDependency::CannotFindReliquaryDependency(int id) :
        Exception("Cannot find dependent " + ::Chroma::ToString(id) + ".")
    {}

    CannotFindReliquaryDependency::CannotFindReliquaryDependency(Reliquary*) :
        Exception("Cannot find dependent reliquary.")
    {}
}