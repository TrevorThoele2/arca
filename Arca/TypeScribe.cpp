#include "TypeScribe.h"

namespace Inscription
{
    void Scribe<Arca::Type, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.name);
        archive(object.isConst);
    }
}