#pragma once

#include <list>

#include "RelicID.h"

#include "Serialization.h"

namespace Arca
{
    class ChildRelicBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~ChildRelicBatchSourceBase() = 0;

        virtual void AddFromBase(void* add) = 0;
        virtual void DestroyFromBase(RelicID id) = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;

        [[nodiscard]] RelicID Parent() const;
    protected:
        explicit ChildRelicBatchSourceBase(RelicID parent);
    private:
        RelicID parent;
    };

    template<class T>
    class ChildRelicBatchSource : public ChildRelicBatchSourceBase
    {
    public:
        using RelicT = T;
    private:
        using List = std::list<RelicT*>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        explicit ChildRelicBatchSource(RelicID parent);

        void Add(RelicT* relic);

        void AddFromBase(void* add) override;

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        void DestroyFromBase(RelicID id) override;

        RelicT* Find(RelicID id);

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        List list;
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    ChildRelicBatchSource<T>::ChildRelicBatchSource(RelicID parent) : ChildRelicBatchSourceBase(parent)
    {}

    template<class T>
    void ChildRelicBatchSource<T>::Add(RelicT* relic)
    {
        auto found = Find(relic->ID());
        if (found)
            return;

        list.push_back(relic);
    }

    template<class T>
    void ChildRelicBatchSource<T>::AddFromBase(void* add)
    {
        auto casted = reinterpret_cast<T*>(add);
        Add(casted);
    }

    template<class T>
    auto ChildRelicBatchSource<T>::Destroy(RelicID destroy) -> iterator
    {
        auto itr = std::remove_if(
            list.begin(),
            list.end(),
            [destroy](const RelicT* entry) { return entry->ID() == destroy; });
        if (itr == list.end())
            return list.end();
        return list.erase(itr);
    }

    template<class T>
    auto ChildRelicBatchSource<T>::Destroy(iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    auto ChildRelicBatchSource<T>::Destroy(const_iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    void ChildRelicBatchSource<T>::DestroyFromBase(RelicID id)
    {
        Destroy(id);
    }

    template<class T>
    auto ChildRelicBatchSource<T>::Find(RelicID id) -> RelicT*
    {
        auto found = std::find_if(
            list.begin(),
            list.end(),
            [id](const RelicT* entry) { return entry->ID() == id; });
        if (found == list.end())
            return {};

        return *found;
    }

    template<class T>
    auto ChildRelicBatchSource<T>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool ChildRelicBatchSource<T>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto ChildRelicBatchSource<T>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto ChildRelicBatchSource<T>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto ChildRelicBatchSource<T>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto ChildRelicBatchSource<T>::end() const -> const_iterator
    {
        return list.end();
    }
}