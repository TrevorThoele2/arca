#include "TypeHandleSerialization.h"

namespace Inscription
{
    void Scribe<Arca::TypeHandle, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.name);
        archive(object.isConst);
    }
}