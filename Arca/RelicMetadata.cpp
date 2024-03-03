#include "RelicMetadata.h"

#include <Inscription/OptionalScribe.h>
#include <Inscription/VectorScribe.h>

namespace Arca
{
    RelicMetadata::RelicMetadata(
        RelicID id,
        RelicOpenness openness,
        TypeHandle typeHandle,
        void* storage)
        :
        id(id), openness(openness), typeHandle(std::move(typeHandle)), storage(storage)
    {}
}