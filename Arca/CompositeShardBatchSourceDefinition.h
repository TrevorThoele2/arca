#pragma once

#include "CompositeShardBatchSource.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    template<::Chroma::VariadicTemplateSize i>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::
        CreateTupleIterator<i>::Do(TupleT& tuple, RelicID id, ReliquaryShards& shards)
    {
        using T = typename Pack::template Parameter<i>::Type;
        std::get<i>(tuple) = shards.Find<T>(id);
    }

    template<class T>
    template<::Chroma::VariadicTemplateSize i>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::
        ToStructure<i>::Do(RelicStructure& structure)
    {
        using T = typename Pack::template Parameter<i>::Type;
        auto typeHandle = TypeHandleFor<T>();
        structure.push_back(std::move(typeHandle));
    }

    template<class T>
    BatchSource<T, std::enable_if_t<is_composite_v<T>>>::BatchSource(ReliquaryShards& owner)
        : owner(&owner)
    {
        ::Chroma::IterateRange<::Chroma::VariadicTemplateSize, ToStructure, Pack::count - 1>(structure);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::Add(RelicID id)
    {
        CreateEntry(id);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::NotifyShardCreated(RelicID id)
    {
        if (HasEntry(id))
            return;

        if (owner->Contains<T>(id))
            CreateEntry(id);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::NotifyShardDestroyed(RelicID id)
    {
        if (!HasEntry(id))
            return;

        if (owner->Contains<T>(id))
            DestroyEntry(id);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::NotifyRelicCreated(
        RelicID id, const RelicStructure& structure)
    {
        for (auto& type : structure)
        {
            if (!std::any_of(
                this->structure.begin(),
                this->structure.end(),
                [type](const TypeHandle& checkType)
                {
                    return checkType == type;
                }))

                return;
        }

        CreateEntry(id);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_composite_v<T>>>::NotifyRelicDestroyed(RelicID id)
    {
        DestroyEntry(id);
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
        ::Chroma::IterateRange<::Chroma::VariadicTemplateSize, CreateTupleIterator, Pack::count - 1>
            (tuple, id, *owner);
        return tuple;
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_composite_v<T>>>::HasEntry(RelicID id) const
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