#pragma once

#include "RelicID.h"

namespace Arca
{
    template<class ReturnT, class WrapT>
    class ChildRelicBatchIteratorBase
    {
    public:
        explicit ChildRelicBatchIteratorBase(const WrapT& wrap);
        ChildRelicBatchIteratorBase(const ChildRelicBatchIteratorBase& arg);

        ChildRelicBatchIteratorBase& operator=(const ChildRelicBatchIteratorBase& arg);

        bool operator==(const ChildRelicBatchIteratorBase& arg) const;
        bool operator!=(const ChildRelicBatchIteratorBase& arg) const;

        ReturnT& operator*();
        ReturnT* operator->();

        ChildRelicBatchIteratorBase& operator++();
        ChildRelicBatchIteratorBase operator++(int) const;
        ChildRelicBatchIteratorBase& operator--();
        ChildRelicBatchIteratorBase operator--(int) const;

        [[nodiscard]] RelicID ID() const;
    private:
        WrapT wrapped;
    private:
        template<class U>
        friend class RelicBatch;
    };

    template<class ReturnT, class WrapT>
    ChildRelicBatchIteratorBase<ReturnT, WrapT>::ChildRelicBatchIteratorBase(
        const WrapT& wrap)
        :
        wrapped(wrap)
    {}

    template<class ReturnT, class WrapT>
    ChildRelicBatchIteratorBase<ReturnT, WrapT>::ChildRelicBatchIteratorBase(const ChildRelicBatchIteratorBase& arg) :
        wrapped(arg.wrapped)
    {}

    template<class ReturnT, class WrapT>
    ChildRelicBatchIteratorBase<ReturnT, WrapT>&
        ChildRelicBatchIteratorBase<ReturnT, WrapT>::operator=(const ChildRelicBatchIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    bool ChildRelicBatchIteratorBase<ReturnT, WrapT>::operator==(const ChildRelicBatchIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class ReturnT, class WrapT>
    bool ChildRelicBatchIteratorBase<ReturnT, WrapT>::operator!=(const ChildRelicBatchIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class ReturnT, class WrapT>
    ReturnT& ChildRelicBatchIteratorBase<ReturnT, WrapT>::operator*()
    {
        return **wrapped;
    }

    template<class ReturnT, class WrapT>
    ReturnT* ChildRelicBatchIteratorBase<ReturnT, WrapT>::operator->()
    {
        return &**wrapped;
    }

    template<class ReturnT, class WrapT>
    ChildRelicBatchIteratorBase<ReturnT, WrapT>&
        ChildRelicBatchIteratorBase<ReturnT, WrapT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    ChildRelicBatchIteratorBase<ReturnT, WrapT>
        ChildRelicBatchIteratorBase<ReturnT, WrapT>::operator++(int) const
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    ChildRelicBatchIteratorBase<ReturnT, WrapT>&
        ChildRelicBatchIteratorBase<ReturnT, WrapT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    ChildRelicBatchIteratorBase<ReturnT, WrapT>
        ChildRelicBatchIteratorBase<ReturnT, WrapT>::operator--(int) const
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicID ChildRelicBatchIteratorBase<ReturnT, WrapT>::ID() const
    {
        return wrapped->id;
    }
}