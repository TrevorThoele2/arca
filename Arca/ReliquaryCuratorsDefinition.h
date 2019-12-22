#pragma once

#include "ReliquaryCurators.h"
#include "Reliquary.h"

namespace Arca
{
    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    Curator* ReliquaryCurators::Find(const TypeName& type)
    {
        const auto found = map.find(type);
        if (found == map.end())
            return nullptr;

        return found->second->Get();
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    const Curator* ReliquaryCurators::Find(const TypeName& type) const
    {
        return const_cast<ReliquaryCurators&>(*this).Find<CuratorT>(type);
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    CuratorT* ReliquaryCurators::Find()
    {
        for (auto& loop : map)
        {
            auto casted = dynamic_cast<CuratorT*>(loop.second->Get());
            if (casted)
                return casted;
        }

        return nullptr;
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    const CuratorT* ReliquaryCurators::Find() const
    {
        return const_cast<ReliquaryCurators*>(this)->Find<CuratorT>();
    }

    template<class CuratorT>
    bool ReliquaryCurators::Contains() const
    {
        return Owner().Find<CuratorT>() != nullptr;
    }
}