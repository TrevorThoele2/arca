#include "CannotFindDependent.h"

#include <Chroma/StringUtility.h>

namespace Arca
{
    CannotFindDependent::CannotFindDependent(int id) :
        Exception("Cannot find dependent " + ::Chroma::ToString(id) + ".")
    {}

    CannotFindDependent::CannotFindDependent(Reliquary*) :
        Exception("Cannot find dependent reliquary.")
    {}
}