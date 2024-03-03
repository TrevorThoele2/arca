#pragma once

#include "RelicExtension.h"
#include "RelicTypeDescription.h"
#include "RelicTraits.h"
#include "ProcessedRelicTraits.h"

namespace Arca
{
    class Reliquary;

    class AnyExtendedRelic
    {
    public:
        const RelicID id;

        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;

        [[nodiscard]] virtual RelicTypeDescription TypeDescription() const = 0;

        [[nodiscard]] RelicExtension Extension() const;

        [[nodiscard]] virtual void* RelicMemory() = 0;
    public:
        virtual ~AnyExtendedRelic() = 0;
    protected:
        AnyExtendedRelic(RelicID id, Reliquary& owner);
    private:
        Reliquary* owner;
        friend Reliquary;
    };

    template<class RelicT>
    class ExtendedRelic : public AnyExtendedRelic
    {
    public:
        RelicT relic;
        [[nodiscard]] RelicTypeDescription TypeDescription() const override;

        [[nodiscard]] void* RelicMemory() override;

        RelicT& operator*();
        const RelicT& operator*() const;
        RelicT* operator->();
        const RelicT* operator->() const;
        operator RelicT&();
        operator const RelicT&() const;
    public:
        ExtendedRelic(const RelicT& relic, RelicID id, Reliquary& owner);
    };

    template<class RelicT>
    RelicTypeDescription ExtendedRelic<RelicT>::TypeDescription() const
    {
        return ProcessedRelicTraits<RelicT>::TypeDescription();
    }

    template<class RelicT>
    void* ExtendedRelic<RelicT>::RelicMemory()
    {
        return &relic;
    }

    template<class RelicT>
    RelicT& ExtendedRelic<RelicT>::operator*()
    {
        return relic;
    }

    template<class RelicT>
    const RelicT& ExtendedRelic<RelicT>::operator*() const
    {
        return relic;
    }

    template<class RelicT>
    RelicT* ExtendedRelic<RelicT>::operator->()
    {
        return &relic;
    }

    template<class RelicT>
    const RelicT* ExtendedRelic<RelicT>::operator->() const
    {
        return &relic;
    }

    template<class RelicT>
    ExtendedRelic<RelicT>::operator RelicT&()
    {
        return relic;
    }

    template<class RelicT>
    ExtendedRelic<RelicT>::operator const RelicT&() const
    {
        return relic;
    }

    template<class RelicT>
    ExtendedRelic<RelicT>::ExtendedRelic(const RelicT& relic, RelicID id, Reliquary& owner) :
        AnyExtendedRelic(id, owner), relic(relic)
    {}
}