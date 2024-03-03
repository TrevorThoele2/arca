#pragma once

#include <type_traits>

#include "Ref.h"
#include "IDManager.h"

#include "Serialization.h"

namespace Arca
{
    class AbstractRelicBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~AbstractRelicBatchSourceBase() = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;
    protected:
        virtual void NotifyAdded(void* relic) = 0;
        virtual void NotifyDestroyed(void* relic) = 0;
        friend class Reliquary;
    };

    template<class T>
    class AbstractRelicBatchSource : public AbstractRelicBatchSourceBase
    {
    public:
        using RelicT = std::add_pointer_t<T>;
        using Reference = Ref<T>;
    private:
        using List = IDManager<RelicID, RelicT>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        AbstractRelicBatchSource() = default;

        Reference Find(RelicID id);

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    protected:
        void NotifyAdded(void* relic) override;
        void NotifyDestroyed(void* relic) override;
    private:
        List list;
    private:
        friend class Reliquary;
        friend class Ref<T>;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    auto AbstractRelicBatchSource<T>::Find(RelicID id) -> Reference
    {
        auto found = list.Find(id);
        if (found == list.end())
            return {};

        return Reference(found.ID(), *this);
    }

    template<class T>
    auto AbstractRelicBatchSource<T>::Size() const -> SizeT
    {
        return list.Size();
    }

    template<class T>
    bool AbstractRelicBatchSource<T>::IsEmpty() const
    {
        return list.IsEmpty();
    }

    template<class T>
    auto AbstractRelicBatchSource<T>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto AbstractRelicBatchSource<T>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto AbstractRelicBatchSource<T>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto AbstractRelicBatchSource<T>::end() const -> const_iterator
    {
        return list.end();
    }

    template<class T>
    void AbstractRelicBatchSource<T>::NotifyAdded(void* relic)
    {
        auto casted = reinterpret_cast<T*>(relic);
        list.Add(casted);
    }

    template<class T>
    void AbstractRelicBatchSource<T>::NotifyDestroyed(void* relic)
    {
        auto casted = reinterpret_cast<T*>(relic);
        list.Remove(std::remove_if(list.begin(), list.end(),
            [casted](auto& entry)
            {
                return casted == entry;
            }));
    }
}