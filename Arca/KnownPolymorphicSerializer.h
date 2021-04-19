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

        [[nodiscard]] virtual bool WillBinarySerialize() const = 0;
        [[nodiscard]] virtual bool WillJsonSerialize() const = 0;
        virtual void Serialize(Inscription::Archive::Binary& archive) = 0;
        virtual void Serialize(const std::string& name, Inscription::Archive::Json& archive) = 0;
        [[nodiscard]] virtual TypeName MainType() const = 0;
        [[nodiscard]] virtual std::vector<Inscription::Type>
            InscriptionTypes(Inscription::Archive::Binary& archive) const = 0;
        [[nodiscard]] virtual std::vector<Inscription::Type>
            InscriptionTypes(Inscription::Archive::Json& archive) const = 0;
    };
}
