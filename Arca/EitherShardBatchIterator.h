#pragma once

#include "RelicID.h"

namespace Arca
{
    template<class ReturnT, class WrapT>
    class EitherShardBatchIteratorBase
    {
    public:
        explicit EitherShardBatchIteratorBase(const WrapT& wrap, const WrapT& end);
        EitherShardBatchIteratorBase(const EitherShardBatchIteratorBase& arg);

        EitherShardBatchIteratorBase& operator=(const EitherShardBatchIteratorBase& arg);

        bool operator==(const EitherShardBatchIteratorBase& arg) const;
        bool operator!=(const EitherShardBatchIteratorBase& arg) const;

        ReturnT& operator*();
        ReturnT* operator->();

        EitherShardBatchIteratorBase& operator++();
        EitherShardBatchIteratorBase operator++(int) const;
        EitherShardBatchIteratorBase& operator--();
        EitherShardBatchIteratorBase operator--(int) const;

        [[nodiscard]] RelicID ID() const;
    private:
        WrapT wrapped;
        WrapT end;
    private:
        template<class U>
        friend class ShardBatch;
    };

    template<class ReturnT, class WrapT>
    EitherShardBatchIteratorBase<ReturnT, WrapT>::EitherShardBatchIteratorBase(
        const WrapT& wrap, const WrapT& end)
        :
        wrapped(wrap),
        end(end)
    {}

    template<class ReturnT, class WrapT>
    EitherShardBatchIteratorBase<ReturnT, WrapT>::EitherShardBatchIteratorBase(const EitherShardBatchIteratorBase& arg) :
        wrapped(arg.wrapped)
    {}

    template<class ReturnT, class WrapT>
    EitherShardBatchIteratorBase<ReturnT, WrapT>&
        EitherShardBatchIteratorBase<ReturnT, WrapT>::operator=(const EitherShardBatchIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    bool EitherShardBatchIteratorBase<ReturnT, WrapT>::operator==(const EitherShardBatchIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class ReturnT, class WrapT>
    bool EitherShardBatchIteratorBase<ReturnT, WrapT>::operator!=(const EitherShardBatchIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class ReturnT, class WrapT>
    ReturnT& EitherShardBatchIteratorBase<ReturnT, WrapT>::operator*()
    {
        return *wrapped->shard;
    }

    template<class ReturnT, class WrapT>
    ReturnT* EitherShardBatchIteratorBase<ReturnT, WrapT>::operator->()
    {
        return wrapped->shard;
    }

    template<class ReturnT, class WrapT>
    EitherShardBatchIteratorBase<ReturnT, WrapT>&
        EitherShardBatchIteratorBase<ReturnT, WrapT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    EitherShardBatchIteratorBase<ReturnT, WrapT>
        EitherShardBatchIteratorBase<ReturnT, WrapT>::operator++(int) const
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    EitherShardBatchIteratorBase<ReturnT, WrapT>&
        EitherShardBatchIteratorBase<ReturnT, WrapT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    EitherShardBatchIteratorBase<ReturnT, WrapT>
        EitherShardBatchIteratorBase<ReturnT, WrapT>::operator--(int) const
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicID EitherShardBatchIteratorBase<ReturnT, WrapT>::ID() const
    {
        return wrapped->id;
    }
}