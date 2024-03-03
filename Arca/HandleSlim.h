#pragma once

#include "RelicID.h"
#include "TypeHandle.h"

namespace Arca
{
    class HandleSlim
    {
    public:
        HandleSlim() = default;
        HandleSlim(RelicID id, TypeHandle type);

        bool operator==(const HandleSlim& arg) const;
        bool operator!=(const HandleSlim& arg) const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] TypeHandle Type() const;
    private:
        RelicID id = 0;
        Arca::TypeHandle type;
    private:
        INSCRIPTION_ACCESS;
    };
}