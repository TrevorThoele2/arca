#pragma once

#include "RelicID.h"
#include "Type.h"

#include "Serialization.h"

namespace Arca
{
    class HandleSlim
    {
    public:
        HandleSlim() = default;
        HandleSlim(RelicID id, Type type);

        bool operator==(const HandleSlim& arg) const;
        bool operator!=(const HandleSlim& arg) const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Type Type() const;
    private:
        RelicID id = 0;
        Arca::Type type;
    private:
        INSCRIPTION_ACCESS;
    };
}