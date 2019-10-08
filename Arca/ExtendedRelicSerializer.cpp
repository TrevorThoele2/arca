#include "ExtendedRelicSerializer.h"

namespace Arca
{
    ExtendedRelicSerializer::Base::~Base() = default;
}

namespace Inscription
{
    void Scribe<::Arca::ExtendedRelicSerializer, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        if (archive.IsOutput())
            object.base->Save(object.relics, *archive.AsOutput());
        else
            object.base->Load(object.relics, object.owner, *archive.AsInput());
    }
}