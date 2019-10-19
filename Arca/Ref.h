#pragma once

#include "RelicID.h"

namespace Arca
{
    template<class T>
    class RelicBatchSource;

    template<class T>
    class Ref
    {
    public:
        using ValueT = std::remove_const_t<T>;
    public:
        Ref();
        Ref(RelicID id, RelicBatchSource<T>& batchSource);

        bool operator==(const Ref& arg) const;
        bool operator==(const ValueT* arg) const;
        bool operator!=(const Ref& arg) const;
        bool operator!=(const ValueT* arg) const;

        operator bool() const;

        ValueT& operator*();
        const ValueT& operator*() const;

        ValueT* operator->();
        const ValueT* operator->() const;
    private:
        RelicID id;
        RelicBatchSource<ValueT>* batchSource;
    private:
        template<class U, typename std::enable_if_t<!std::is_abstract_v<U>, int> = 0>
        ValueT* Extract() const;
        template<class U, typename std::enable_if_t<std::is_abstract_v<U>, int> = 0>
        ValueT* Extract() const;
    private:
        friend class RelicBatchSource<T>;
    };

    template<class T>
    Ref<T>::Ref() : id(nullRelicID), batchSource(nullptr)
    {}

    template<class T>
    Ref<T>::Ref(RelicID id, RelicBatchSource<T>& batchSource) : id(id), batchSource(&batchSource)
    {}

    template<class T>
    bool Ref<T>::operator==(const Ref& arg) const
    {
        return id == arg.id && batchSource == arg.batchSource;
    }

    template<class T>
    bool Ref<T>::operator==(const ValueT* arg) const
    {
        if (!*this && arg == nullptr)
            return true;

        return &**this == arg;
    }

    template<class T>
    bool Ref<T>::operator!=(const Ref& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    bool Ref<T>::operator!=(const ValueT* arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Ref<T>::operator bool() const
    {
        return id != nullRelicID;
    }

    template<class T>
    auto Ref<T>::operator*() -> ValueT&
    {
        return *Extract<T>();
    }

    template<class T>
    auto Ref<T>::operator*() const -> const ValueT&
    {
        return *Extract<T>();
    }

    template<class T>
    auto Ref<T>::operator->() -> ValueT*
    {
        return Extract<T>();
    }

    template<class T>
    auto Ref<T>::operator->() const -> const ValueT*
    {
        return Extract<T>();
    }

    template<class T>
    template<class U, typename std::enable_if_t<!std::is_abstract_v<U>, int>>
    auto Ref<T>::Extract() const -> ValueT*
    {
        auto found = batchSource->list.Find(id);
        if (found == batchSource->list.end())
            return nullptr;

        return &*found;
    }

    template<class T>
    template<class U, typename std::enable_if_t<std::is_abstract_v<U>, int>>
    auto Ref<T>::Extract() const -> ValueT*
    {
        auto found = batchSource->list.Find(id);
        if (found == batchSource->list.end())
            return nullptr;

        return found;
    }
}