#pragma once

#include <type_traits>

#include "Ref.h"
#include "IDManager.h"

#include "Serialization.h"

namespace Arca
{
    class RelicBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~RelicBatchSourceBase() = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;
    protected:
        friend class Reliquary;
    };

    template<class T>
    class RelicBatchSource : public RelicBatchSourceBase
    {
    public:
        using RelicT = T;
        using Reference = Ref<T>;
    private:
        using List = IDManager<RelicID, RelicT>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        RelicBatchSource() = default;

        Reference Add(const RelicT& add);
        Reference Add(RelicT&& add);

        void Destroy(Reference destroy);
        void Destroy(iterator destroy);
        void Destroy(const_iterator destroy);

        Reference Find(RelicID id);

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
        friend class Ref<T>;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    auto RelicBatchSource<T>::Add(const RelicT& add) -> Reference
    {
        auto added = list.Add(add);
        return Reference(added.ID(), *this);
    }

    template<class T>
    auto RelicBatchSource<T>::Add(RelicT&& add) -> Reference
    {
        auto added = list.Add(std::move(add));
        return Reference(added.ID(), *this);
    }

    template<class T>
    void RelicBatchSource<T>::Destroy(Reference destroy)
    {
        if (destroy.batchSource != this)
            return;

        list.Remove(destroy.id);
    }

    template<class T>
    void RelicBatchSource<T>::Destroy(iterator destroy)
    {
        list.Remove(destroy);
    }

    template<class T>
    void RelicBatchSource<T>::Destroy(const_iterator destroy)
    {
        list.Destroy(destroy);
    }

    template<class T>
    auto RelicBatchSource<T>::Find(RelicID id) -> Reference
    {
        auto found = list.Find(id);
        if (found == list.end())
            return {};

        return Reference(found.ID(), *this);
    }

    template<class T>
    auto RelicBatchSource<T>::Size() const -> SizeT
    {
        return list.Size();
    }

    template<class T>
    bool RelicBatchSource<T>::IsEmpty() const
    {
        return list.IsEmpty();
    }

    template<class T>
    auto RelicBatchSource<T>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto RelicBatchSource<T>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto RelicBatchSource<T>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto RelicBatchSource<T>::end() const -> const_iterator
    {
        return list.end();
    }
}