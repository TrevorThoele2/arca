#pragma once

#include "TypeHandle.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    struct KnownPolymorphicSerializer
    {
        const TypeHandleName mainTypeHandle;

        using Serializer = std::function<void(Reliquary&, ::Inscription::BinaryArchive&)>;
        const Serializer serializer;

        using InscriptionTypeHandles = std::vector<::Inscription::TypeHandle>;
        using InscriptionTypeHandleProvider =
            std::function<InscriptionTypeHandles(::Inscription::BinaryArchive&)>;
        InscriptionTypeHandleProvider inscriptionTypeProvider;

        KnownPolymorphicSerializer(
            TypeHandleName mainTypeHandle,
            Serializer&& serializer,
            InscriptionTypeHandleProvider&& inscriptionTypeProvider);
    };
}
