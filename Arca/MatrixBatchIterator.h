#pragma once

#include "RelicID.h"

namespace Arca
{
    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    class MatrixBatchIteratorBase
    {
    public:
        explicit MatrixBatchIteratorBase(const WrapT& wrap, const WrapT& end);
        MatrixBatchIteratorBase(const MatrixBatchIteratorBase& arg);

        MatrixBatchIteratorBase& operator=(const MatrixBatchIteratorBase& arg);

        bool operator==(const MatrixBatchIteratorBase& arg) const;
        bool operator!=(const MatrixBatchIteratorBase& arg) const;

        ReturnReferenceT operator*();
        ReturnOptionalT operator->();

        MatrixBatchIteratorBase& operator++();
        MatrixBatchIteratorBase operator++(int) const;
        MatrixBatchIteratorBase& operator--();
        MatrixBatchIteratorBase operator--(int) const;

        [[nodiscard]] RelicID ID() const;
    private:
        WrapT wrapped;
        WrapT end;
    private:
        template<class U>
        friend class ShardBatch;
    };

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::MatrixBatchIteratorBase(
        const WrapT& wrap, const WrapT& end)
        :
        wrapped(wrap),
        end(end)
    {}

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::MatrixBatchIteratorBase(const MatrixBatchIteratorBase& arg) :
        wrapped(arg.wrapped)
    {}

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>&
        MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::operator=(const MatrixBatchIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    bool MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::operator==(const MatrixBatchIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    bool MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::operator!=(const MatrixBatchIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    ReturnReferenceT MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::operator*()
    {
        return wrapped->Reference();
    }

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    ReturnOptionalT MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::operator->()
    {
        return wrapped->Optional();
    }

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>&
        MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>
        MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::operator++(int) const
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>&
        MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>
        MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::operator--(int) const
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class ReturnReferenceT, class ReturnOptionalT, class WrapT>
    RelicID MatrixBatchIteratorBase<ReturnReferenceT, ReturnOptionalT, WrapT>::ID() const
    {
        return wrapped->id;
    }
}