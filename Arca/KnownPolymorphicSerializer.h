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

        using InscriptionTypes = std::vector<::Inscription::Type>;
        using InscriptionTypeProvider =
            std::function<InscriptionTypes(::Inscription::BinaryArchive&)>;
        InscriptionTypeProvider inscriptionTypeProvider;

        KnownPolymorphicSerializer(
            TypeName mainType,
            Serializer&& serializer,
            InscriptionTypeProvider&& inscriptionTypeProvider);
    };
}
