#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    KnownPolymorphicSerializer::KnownPolymorphicSerializer(
        TypeHandleName mainTypeHandle,
        Serializer&& serializer,
        InscriptionTypeHandleProvider&& inscriptionTypeProvider)
        :
        mainTypeHandle(std::move(mainTypeHandle)),
        serializer(std::move(serializer)),
        inscriptionTypeProvider(std::move(inscriptionTypeProvider))
    {}
}