#pragma once

#include "RelicBatchSource.h"

namespace Arca
{
    template<class RelicT>
    RelicBatchSource<RelicT>::RelicBatchSource(Reliquary& owner) : RelicBatchSourceBase(owner)
    {
        SetupAssociatedDescriptionGroup();
        Synchronize();
    }

    template<class RelicT>
    RelicBatchSource<RelicT>::RelicBatchSource(const Inscription::BinaryTableData<RelicBatchSource>& data) :
        RelicBatchSourceBase(*data.owner)
    {
        SetupAssociatedDescriptionGroup();
        Synchronize();
    }

    template<class RelicT>
    RelicBatchSource<RelicT>::~RelicBatchSource()
    {
        onInvalidated();
    }

    template<class RelicT>
    auto RelicBatchSource<RelicT>::Remove(iterator itr) -> iterator
    {
        if (itr == end())
            return end();

        auto next = std::next(itr);
        owner->DestroyRelic(**itr);
        return next;
    }

    template<class RelicT>
    auto RelicBatchSource<RelicT>::Remove(const_iterator itr) -> iterator
    {
        if (itr == end())
            return end();

        auto next = std::next(itr);
        owner->DestroyRelic(*itr);
        return next;
    }

    template<class RelicT>
    auto RelicBatchSource<RelicT>::Find(RelicID id) -> iterator
    {
        for (auto loop = extendedList.begin(); loop != extendedList.end(); ++loop)
            if (loop->second.id == id)
                return loop->first;

        return list.end();
    }

    template<class RelicT>
    auto RelicBatchSource<RelicT>::Find(RelicID id) const -> const_iterator
    {
        for (auto loop = extendedList.begin(); loop != extendedList.end(); ++loop)
            if (loop->second.id == id)
                return loop.first;

        return list.end();
    }

    template<class RelicT>
    auto RelicBatchSource<RelicT>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class RelicT>
    bool RelicBatchSource<RelicT>::IsEmpty() const
    {
        return list.empty();
    }

    template<class RelicT>
    auto RelicBatchSource<RelicT>::begin() -> iterator
    {
        return list.begin();
    }

    template<class RelicT>
    auto RelicBatchSource<RelicT>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class RelicT>
    auto RelicBatchSource<RelicT>::end() -> iterator
    {
        return list.end();
    }

    template<class RelicT>
    auto RelicBatchSource<RelicT>::end() const -> const_iterator
    {
        return list.end();
    }

    template<class RelicT>
    void RelicBatchSource<RelicT>::NotifyCreated(AnyExtendedRelic& relic)
    {
        AttemptAddToList(relic);
    }

    template<class RelicT>
    void RelicBatchSource<RelicT>::NotifyDestroyed(AnyExtendedRelic& relic)
    {
        if (!IsManaging(relic.TypeDescription().typeHandle))
            return;

        auto memory = relic.RelicMemory();
        for (auto loop = extendedList.begin(); loop != extendedList.end(); ++loop)
        {
            if (*loop->first == memory)
            {
                list.erase(loop->first);
                extendedList.erase(loop);
                return;
            }
        }
    }

    template<class RelicT>
    void RelicBatchSource<RelicT>::IncrementReference()
    {
        ++referenceCount;
    }

    template<class RelicT>
    void RelicBatchSource<RelicT>::DecrementReference()
    {
        --referenceCount;
        if (referenceCount == 0)
            owner->DestroyRelicBatchSource<RelicT>();
    }

    template<class RelicT>
    void RelicBatchSource<RelicT>::AttemptAddToList(AnyExtendedRelic& relic)
    {
        if (!IsManaging(relic.TypeDescription().typeHandle))
            return;

        auto memory = relic.RelicMemory();
        auto extension = relic.Extension();
        list.push_back(static_cast<RelicT*>(memory));
        extendedList.push_back(ExtendedPair{ --list.end(), extension });
    }

    template<class RelicT>
    void RelicBatchSource<RelicT>::Synchronize()
    {
        for (auto& loop : owner->relics)
            AttemptAddToList(*loop.get());
    }

    template<class RelicT>
    void RelicBatchSource<RelicT>::SetupAssociatedDescriptionGroup()
    {
        associatedDescriptionGroup = owner->DescriptionGroupForRelic(RelicTraits<RelicT>::typeHandle);
    }

    template<class RelicT>
    bool RelicBatchSource<RelicT>::IsManaging(const TypeHandle& checkAgainst) const
    {
        return associatedDescriptionGroup.IsIn(checkAgainst);
    }

    template<class RelicT>
    bool RelicBatchSource<RelicT>::IsManaging(RelicID checkAgainst) const
    {
        for (auto& loop : list)
            if (loop->id == checkAgainst)
                return true;

        return false;
    }
}