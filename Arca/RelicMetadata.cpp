#include "RelicMetadata.h"

namespace Arca
{
    RelicMetadata::RelicMetadata(
        RelicID id,
        Locality locality,
        Type type,
        void* storage,
        bool shouldSerializeBinary,
        bool shouldSerializeJson)
        :
        id(id),
        locality(locality),
        type(std::move(type)),
        storage(storage),
        shouldSerializeBinary(shouldSerializeBinary),
        shouldSerializeJson(shouldSerializeJson)
    {}
}