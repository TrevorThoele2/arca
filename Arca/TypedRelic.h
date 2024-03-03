#pragma once

#include "RelicID.h"
#include "RelicStructure.h"
#include "RelicScribe.h"
#include "RelicTraits.h"
#include "Handle.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;
    class OpenRelic;

    class TypedRelic
    {
    public:
        operator Handle() const;
        explicit operator bool() const;

        [[nodiscard]] std::optional<Handle> Parent() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    public:
        virtual ~TypedRelic() = 0;

        void Initialize(Reliquary& owner);
    protected:
        TypedRelic() = default;

        virtual void InitializeImplementation() {}
        [[nodiscard]] virtual bool ReliquaryContainsSelf() const = 0;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        friend class Reliquary;
        friend class ReliquaryOrigin;
        template<class T, class>
        friend class BatchSource;
    private:
        INSCRIPTION_ACCESS;
    };
}