#pragma once

#include <optional>

#include "RelicID.h"

#include "Handle.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class ClosedTypedRelic
    {
    public:
        explicit operator bool() const;

        [[nodiscard]] std::optional<Handle> Parent() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    public:
        virtual ~ClosedTypedRelic() = 0;
    protected:
        ClosedTypedRelic() = default;
        ClosedTypedRelic(ClosedTypedRelic&& arg) noexcept = default;

        [[nodiscard]] virtual bool ReliquaryContainsSelf() const = 0;
        [[nodiscard]] virtual Type Type() const = 0;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        friend class ReliquaryRelics;
        friend class ReliquaryOrigin;
        template<class, class>
        friend class BatchSource;
    private:
        INSCRIPTION_ACCESS;
    };
}