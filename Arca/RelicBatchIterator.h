#pragma once

#include "RelicID.h"
#include "Ref.h"

namespace Arca
{
    template<class ReturnT, class WrapT, class BatchSourceT>
    class RelicBatchIteratorBase
    {
    public:
        explicit RelicBatchIteratorBase(const WrapT& wrap, BatchSourceT& batchSource);
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

        [[nodiscard]] Ref<ReturnT> ToRef();
    private:
        WrapT wrapped;
        BatchSourceT* batchSource;
    private:
        template<class U>
        friend class RelicBatch;
    };

    template<class ReturnT, class WrapT, class BatchSourceT>
    RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::RelicBatchIteratorBase(
        const WrapT& wrap, BatchSourceT& batchSource)
        :
        wrapped(wrap), batchSource(&batchSource)
    {}

    template<class ReturnT, class WrapT, class BatchSourceT>
    RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::RelicBatchIteratorBase(const RelicBatchIteratorBase& arg) :
        wrapped(arg.wrapped)
    {}

    template<class ReturnT, class WrapT, class BatchSourceT>
    RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>&
        RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::operator=(const RelicBatchIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    bool RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::operator==(const RelicBatchIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    bool RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::operator!=(const RelicBatchIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    ReturnT& RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::operator*()
    {
        return *wrapped;
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    ReturnT* RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::operator->()
    {
        return &*wrapped;
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>&
        RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>
        RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::operator++(int) const
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>&
        RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>
        RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::operator--(int) const
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    RelicID RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::ID() const
    {
        return wrapped.ID();
    }

    template<class ReturnT, class WrapT, class BatchSourceT>
    [[nodiscard]] Ref<ReturnT> RelicBatchIteratorBase<ReturnT, WrapT, BatchSourceT>::ToRef()
    {
        return Ref(wrapped->id, *batchSource);
    }
}