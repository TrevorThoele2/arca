#pragma once

#include "Type.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class KnownPolymorphicSerializer
    {
    public:
        virtual ~KnownPolymorphicSerializer() = 0;

        virtual bool WillSerialize() const = 0;
        virtual void Serialize(Inscription::BinaryArchive& archive) = 0;
        [[nodiscard]] virtual TypeName MainType() const = 0;
        [[nodiscard]] virtual std::vector<::Inscription::Type>
            InscriptionTypes(Inscription::BinaryArchive& archive) const = 0;
    };
}
