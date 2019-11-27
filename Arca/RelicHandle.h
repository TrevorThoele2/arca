#pragma once

#include "RelicID.h"

namespace Arca
{
    class Reliquary;
    class DynamicRelic;

    class RelicHandle
    {
    public:
        RelicHandle() = default;
        RelicHandle(RelicID id, Reliquary& owner);

        bool operator==(const RelicHandle& arg) const;
        bool operator!=(const RelicHandle& arg) const;

        [[nodiscard]] Reliquary& Owner() const;
        [[nodiscard]] RelicID ID() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    };
}
