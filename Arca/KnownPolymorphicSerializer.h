#pragma once

#include "Type.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    struct KnownPolymorphicSerializer
    {
        const TypeName mainType;

        using Serializer = std::function<void(Reliquary&, ::Inscription::BinaryArchive&)>;
        const Serializer serializer;

        using InscriptionTypeHandles = std::vector<::Inscription::TypeHandle>;
        using InscriptionTypeHandleProvider =
            std::function<InscriptionTypeHandles(::Inscription::BinaryArchive&)>;
        InscriptionTypeHandleProvider inscriptionTypeProvider;

        KnownPolymorphicSerializer(
            TypeName mainType,
            Serializer&& serializer,
            InscriptionTypeHandleProvider&& inscriptionTypeProvider);
    };
}
