#pragma once

#include "RelicID.h"

namespace Arca
{
    template<class ReturnT, class WrapT>
    class RelicBatchIteratorBase
    {
    public:
        explicit RelicBatchIteratorBase(const WrapT& wrap);
        RelicBatchIteratorBase(const RelicBatchIteratorBase& arg);

        RelicBatchIteratorBase& operator=(const RelicBatchIteratorBase& arg);

        bool operator==(const RelicBatchIteratorBase& arg) const;
        bool operator!=(const RelicBatchIteratorBase& arg) const;

        ReturnT& operator*();
        ReturnT* operator->();

        RelicBatchIteratorBase& operator++();
        RelicBatchIteratorBase operator++(int) const;
        RelicBatchIteratorBase& operator--();
        RelicBatchIteratorBase operator--(int) const;

        [[nodiscard]] RelicID ID() const;
    private:
        WrapT wrapped;
    private:
        template<class U>
        friend class RelicBatch;
    };

    template<class ReturnT, class WrapT>
    RelicBatchIteratorBase<ReturnT, WrapT>::RelicBatchIteratorBase(
        const WrapT& wrap)
        :
        wrapped(wrap)
    {}

    template<class ReturnT, class WrapT>
    RelicBatchIteratorBase<ReturnT, WrapT>::RelicBatchIteratorBase(const RelicBatchIteratorBase& arg) :
        wrapped(arg.wrapped)
    {}

    template<class ReturnT, class WrapT>
    RelicBatchIteratorBase<ReturnT, WrapT>&
        RelicBatchIteratorBase<ReturnT, WrapT>::operator=(const RelicBatchIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    bool RelicBatchIteratorBase<ReturnT, WrapT>::operator==(const RelicBatchIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class ReturnT, class WrapT>
    bool RelicBatchIteratorBase<ReturnT, WrapT>::operator!=(const RelicBatchIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class ReturnT, class WrapT>
    ReturnT& RelicBatchIteratorBase<ReturnT, WrapT>::operator*()
    {
        return wrapped->second;
    }

    template<class ReturnT, class WrapT>
    ReturnT* RelicBatchIteratorBase<ReturnT, WrapT>::operator->()
    {
        return &wrapped->second;
    }

    template<class ReturnT, class WrapT>
    RelicBatchIteratorBase<ReturnT, WrapT>&
        RelicBatchIteratorBase<ReturnT, WrapT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    RelicBatchIteratorBase<ReturnT, WrapT>
        RelicBatchIteratorBase<ReturnT, WrapT>::operator++(int) const
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicBatchIteratorBase<ReturnT, WrapT>&
        RelicBatchIteratorBase<ReturnT, WrapT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    RelicBatchIteratorBase<ReturnT, WrapT>
        RelicBatchIteratorBase<ReturnT, WrapT>::operator--(int) const
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicID RelicBatchIteratorBase<ReturnT, WrapT>::ID() const
    {
        return wrapped->id;
    }
}