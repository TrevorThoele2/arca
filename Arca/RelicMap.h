#pragma once

#include <unordered_map>

#include "RelicMapIterator.h"

namespace Arca
{
    template<class RelicT>
    class RelicMap
    {
    public:
        using SizeT = size_t;
    private:
        using ReferenceMap = std::unordered_map<RelicID, RelicT*>;

        using BasicIterator = typename ReferenceMap::iterator;
        using ConstBasicIterator = typename ReferenceMap::const_iterator;
    public:
        using iterator = RelicMapIteratorBase<RelicT*, BasicIterator>;
        using const_iterator = RelicMapIteratorBase<const RelicT*, ConstBasicIterator>;
    public:
        RelicMap();

        void Add(RelicT* add);
        void Insert(iterator from, iterator to);

        bool Remove(RelicID remove);
        bool Remove(RelicT* remove);
        bool Remove(iterator remove);
        bool Remove(const_iterator remove);
        void Clear();

        [[nodiscard]] bool Has(RelicID check) const;
        [[nodiscard]] bool Has(RelicT* check) const;

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        ReferenceMap map;
    private:
        bool RemoveByID(RelicID remove);
        bool RemoveByIterator(BasicIterator remove);
    private:
        iterator Wrap(BasicIterator itr);
        const_iterator Wrap(ConstBasicIterator itr) const;
        static BasicIterator Unwrap(iterator itr);
        static ConstBasicIterator Unwrap(const_iterator itr);
    };

    template<class RelicT>
    RelicMap<RelicT>::RelicMap()
    {}

    template<class RelicT>
    void RelicMap<RelicT>::Add(RelicT* add)
    {
        map.emplace(add->ID(), add);
    }

    template<class RelicT>
    void RelicMap<RelicT>::Insert(iterator from, iterator to)
    {
        map.insert(Unwrap(from), Unwrap(to));
    }

    template<class RelicT>
    bool RelicMap<RelicT>::Remove(RelicID remove)
    {
        return RemoveByID(remove);
    }

    template<class RelicT>
    bool RelicMap<RelicT>::Remove(RelicT* remove)
    {
        return RemoveByID(remove->ID());
    }

    template<class RelicT>
    bool RelicMap<RelicT>::Remove(iterator remove)
    {
        return RemoveByIterator(Unwrap(remove));
    }

    template<class RelicT>
    bool RelicMap<RelicT>::Remove(const_iterator remove)
    {
        return RemoveByIterator(remove);
    }

    template<class RelicT>
    void RelicMap<RelicT>::Clear()
    {
        map.clear();
    }

    template<class RelicT>
    bool RelicMap<RelicT>::Has(RelicID check) const
    {
        return Unwrap(Find(check)) != map.end();
    }

    template<class RelicT>
    bool RelicMap<RelicT>::Has(RelicT* check) const
    {
        return Has(check->ID());
    }

    template<class RelicT>
    typename RelicMap<RelicT>::SizeT RelicMap<RelicT>::Size() const
    {
        return map.size();
    }

    template<class RelicT>
    bool RelicMap<RelicT>::IsEmpty() const
    {
        return map.empty();
    }

    template<class RelicT>
    typename RelicMap<RelicT>::iterator RelicMap<RelicT>::begin()
    {
        return Wrap(map.begin());
    }

    template<class RelicT>
    typename RelicMap<RelicT>::const_iterator RelicMap<RelicT>::begin() const
    {
        return Wrap(map.begin());
    }

    template<class RelicT>
    typename RelicMap<RelicT>::iterator RelicMap<RelicT>::end()
    {
        return Wrap(map.end());
    }

    template<class RelicT>
    typename RelicMap<RelicT>::const_iterator RelicMap<RelicT>::end() const
    {
        return Wrap(map.end());
    }

    template<class RelicT>
    bool RelicMap<RelicT>::RemoveByID(RelicID remove)
    {
        return map.erase(remove) != 0;
    }

    template<class RelicT>
    bool RelicMap<RelicT>::RemoveByIterator(BasicIterator remove)
    {
        return RemoveByID(remove.first);
    }

    template<class RelicT>
    auto RelicMap<RelicT>::Wrap(BasicIterator itr) -> iterator
    {
        return iterator(itr);
    }

    template<class RelicT>
    auto RelicMap<RelicT>::Wrap(ConstBasicIterator itr) const -> const_iterator
    {
        return const_iterator(itr);
    }

    template<class RelicT>
    auto RelicMap<RelicT>::Unwrap(iterator itr) -> BasicIterator
    {
        return itr.wrapped;
    }

    template<class RelicT>
    auto RelicMap<RelicT>::Unwrap(const_iterator itr) -> ConstBasicIterator
    {
        return itr.wrapped;
    }
}