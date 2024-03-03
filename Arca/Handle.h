#pragma once

#include "RelicID.h"
#include "TypeHandle.h"
#include "HandleSlim.h"

namespace Arca
{
    class Reliquary;

    class Handle
    {
    public:
        Handle() = default;
        Handle(RelicID id, Reliquary& owner, TypeHandle type);

        bool operator==(const Handle& arg) const;
        bool operator!=(const Handle& arg) const;

        operator HandleSlim() const;

        [[nodiscard]] Reliquary& Owner() const;
        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] TypeHandle Type() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
        Arca::TypeHandle type;
    };
}