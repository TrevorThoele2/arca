#pragma once

#include "Vessel.h"
#include "Reliquary.h"

namespace Arca
{
    template<class RelicT>
    RelicT* Vessel::CreateRelic()
    {
        return owner->CreateRelic<RelicT>(id);
    }

    template<class RelicT>
    void Vessel::DestroyRelic()
    {
        owner->DestroyRelic<RelicT>(id);
    }

    template<class RelicT>
    RelicT* Vessel::FindRelic()
    {
        return owner->FindRelic<RelicT>(id);
    }

    template<class RelicT>
    const RelicT* Vessel::FindRelic() const
    {
        return owner->FindRelic<RelicT>(id);
    }

    template<class RelicT>
    bool Vessel::HasRelic() const
    {
        return FindRelic<RelicT>() != nullptr;
    }
}