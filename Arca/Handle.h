#pragma once

#include "RelicID.h"

namespace Arca
{
    class Reliquary;

    class Handle
    {
    public:
        Handle() = default;
        Handle(RelicID id, Reliquary& owner);

        bool operator==(const Handle& arg) const;
        bool operator!=(const Handle& arg) const;

        [[nodiscard]] Reliquary& Owner() const;
        [[nodiscard]] RelicID ID() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    };
}
