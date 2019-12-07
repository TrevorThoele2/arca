#include "RelicMetadata.h"

namespace Arca
{
    RelicMetadata::RelicMetadata(
        RelicID id,
        Openness openness,
        Locality locality,
        TypeHandle typeHandle,
        void* storage)
        :
        id(id), openness(openness), locality(locality), typeHandle(std::move(typeHandle)), storage(storage)
    {}
}