#include "VesselMetadata.h"

#include <Inscription/OptionalScribe.h>
#include <Inscription/VectorScribe.h>

namespace Arca
{
    VesselMetadata::VesselMetadata(
        VesselID id,
        VesselDynamism dynamism,
        std::optional<TypeHandle> typeHandle)
        :
        id(id), dynamism(dynamism), typeHandle(std::move(typeHandle))
    {}
}

namespace Inscription
{
    void Scribe<::Arca::VesselMetadata, BinaryArchive>::ScrivenImplementation(
        ObjectT& object, ArchiveT& archive)
    {
        archive(object.id);
        archive(object.dynamism);
        archive(object.typeHandle);
        archive(object.parent);
        archive(object.children);
    }
}