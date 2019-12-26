#pragma once

#include "RelicBatchSource.h"
#include "Reliquary.h"
#include "PostConstruct.h"

namespace Arca
{
    template<class T>
    BatchSource<T, std::enable_if_t<is_relic_v<T>>>::BatchSource(Reliquary& owner) : owner(&owner)
    {}

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Add(RelicT&& relic) -> RelicT*
    {
        auto found = Find(relic.ID());
        if (found)
            return nullptr;

        list.push_back(std::move(relic));
        return &list.back();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(RelicID destroy) -> iterator
    {
        for (auto loop = list.begin(); loop != list.end(); ++loop)
            if (loop->ID() == destroy)
                return list.erase(loop);

        return list.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(const_iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_relic_v<T>>>::DestroyFromBase(RelicID id)
    {
        Destroy(id);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_relic_v<T>>>::DestroyAllFromBase(Reliquary& reliquary)
    {
        reliquary.Clear<T>();
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Construct(Reliquary& owner)
    {
        for (auto& loop : list)
        {
            loop.owner = &owner;
            PostConstruct(loop);
        }
    }

    template<class T>
    void* BatchSource<T, std::enable_if_t<is_relic_v<T>>>::FindStorage(RelicID id)
    {
        auto found = std::find_if(
            list.begin(),
            list.end(),
            [id](const RelicT& entry) { return entry.ID() == id; });
        if (found == list.end())
            return {};

        return &*found;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Find(RelicID id) -> RelicT*
    {
        return static_cast<RelicT*>(FindStorage(id));
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_relic_v<T>>>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::end() const -> const_iterator
    {
        return list.end();
    }

    template<class T>
    Arca::Type BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Type() const
    {
        return TypeFor<T>();
    }
}