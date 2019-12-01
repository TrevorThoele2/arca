#include "RelicMetadata.h"

#include <Inscription/OptionalScribe.h>
#include <Inscription/VectorScribe.h>

namespace Arca
{
    RelicMetadata::RelicMetadata(
        RelicID id,
        RelicOpenness openness,
        std::optional<TypeHandle> typeHandle,
        void* storage)
        :
        id(id), openness(openness), typeHandle(std::move(typeHandle)), storage(storage)
    {}
}

namespace Inscription
{
    void Scribe<::Arca::RelicMetadata, BinaryArchive>::ScrivenImplementation(
        ObjectT& object, ArchiveT& archive)
    {
        archive(object.id);
        archive(object.openness);
        archive(object.typeHandle);
        archive(object.parent);
        archive(object.children);
    }
}