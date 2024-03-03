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
        virtual void Serialize(Inscription::Format::Binary& format) = 0;
        virtual void Serialize(const std::string& name, Inscription::Format::Json& format) = 0;
        [[nodiscard]] virtual TypeName MainType() const = 0;
        [[nodiscard]] virtual std::vector<Inscription::Type>
            InscriptionTypes(Inscription::Format::Binary& format) const = 0;
        [[nodiscard]] virtual std::vector<Inscription::Type>
            InscriptionTypes(Inscription::Format::Json& format) const = 0;
    };
}
