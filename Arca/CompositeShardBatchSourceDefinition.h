#pragma once

#include "CompositeShardBatchSource.h"
#include "ReliquaryShards.h"
#include "Composite.h"

namespace Arca
{
    template<class T>
    BatchSource<T, std::enable_if_t<is_composite_v<T>>>::BatchSource(ReliquaryShards& owner)
        : owner(&owner)
    {}

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::Add(RelicID id)
    {
        CreateEntry(id);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::NotifyShardCreated(RelicID id)
    {
        if (ContainsEntry(id))
            return;

        if (owner->Contains<T>(id))
            CreateEntry(id);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::NotifyShardDestroyed(RelicID id)
    {
        if (!ContainsEntry(id))
            return;

        if (!owner->Contains<T>(id))
            DestroyEntry(id);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::Clear()
    {
        list.clear();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_composite_v<T>>>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_composite_v<T>>>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_composite_v<T>>>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_composite_v<T>>>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_composite_v<T>>>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_composite_v<T>>>::end() const -> const_iterator
    {
        return list.end();
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::Reference()
    {
        ++referenceCount;
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::Dereference()
    {
        --referenceCount;
        if (referenceCount == 0)
            owner->compositeBatchSources.map.erase(typeid(T));
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::CreateEntry(RelicID id)
    {
        list.push_back({ id, CreateTuple(id) });
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::DestroyEntry(RelicID id)
    {
        auto itr = std::remove_if(
            list.begin(),
            list.end(),
            [id](const Entry& entry) { return entry.id == id; });
        if (itr == list.end())
            return;
        list.erase(itr);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_composite_v<T>>>::CreateTuple(RelicID id) -> TupleT
    {
        TupleT tuple;
        ::Chroma::IterateRange<::Chroma::VariadicTemplateSize, CompositeToTuple, Pack::count - 1>
            (tuple, id, *owner);
        return tuple;
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_composite_v<T>>>::ContainsEntry(RelicID id) const
    {
        return std::any_of(
            this->list.begin(),
            this->list.end(),
            [id](const Entry& entry)
            {
                return entry.id == id;
            });
    }
}