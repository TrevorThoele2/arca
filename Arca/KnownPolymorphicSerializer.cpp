#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    KnownPolymorphicSerializer::KnownPolymorphicSerializer(
        TypeName mainType,
        Serializer&& serializer,
        InscriptionTypeHandleProvider&& inscriptionTypeProvider)
        :
        mainType(std::move(mainType)),
        serializer(std::move(serializer)),
        inscriptionTypeProvider(std::move(inscriptionTypeProvider))
    {}
}