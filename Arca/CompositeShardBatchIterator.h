#pragma once

#include "RelicID.h"

namespace Arca
{
    template<class ReturnT, class WrapT>
    class AllShardBatchIteratorBase
    {
    public:
        explicit AllShardBatchIteratorBase(const WrapT& wrap, const WrapT& end);
        AllShardBatchIteratorBase(const AllShardBatchIteratorBase& arg);

        AllShardBatchIteratorBase& operator=(const AllShardBatchIteratorBase& arg);

        bool operator==(const AllShardBatchIteratorBase& arg) const;
        bool operator!=(const AllShardBatchIteratorBase& arg) const;

        ReturnT& operator*();
        ReturnT* operator->();

        AllShardBatchIteratorBase& operator++();
        AllShardBatchIteratorBase operator++(int) const;
        AllShardBatchIteratorBase& operator--();
        AllShardBatchIteratorBase operator--(int) const;

        [[nodiscard]] RelicID ID() const;
    private:
        WrapT wrapped;
        WrapT end;
    private:
        template<class U>
        friend class ShardBatch;
    };

    template<class ReturnT, class WrapT>
    AllShardBatchIteratorBase<ReturnT, WrapT>::AllShardBatchIteratorBase(
        const WrapT& wrap, const WrapT& end)
        :
        wrapped(wrap),
        end(end)
    {}

    template<class ReturnT, class WrapT>
    AllShardBatchIteratorBase<ReturnT, WrapT>::AllShardBatchIteratorBase(const AllShardBatchIteratorBase& arg) :
        wrapped(arg.wrapped)
    {}

    template<class ReturnT, class WrapT>
    AllShardBatchIteratorBase<ReturnT, WrapT>&
        AllShardBatchIteratorBase<ReturnT, WrapT>::operator=(const AllShardBatchIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    bool AllShardBatchIteratorBase<ReturnT, WrapT>::operator==(const AllShardBatchIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class ReturnT, class WrapT>
    bool AllShardBatchIteratorBase<ReturnT, WrapT>::operator!=(const AllShardBatchIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class ReturnT, class WrapT>
    ReturnT& AllShardBatchIteratorBase<ReturnT, WrapT>::operator*()
    {
        return wrapped->tuple;
    }

    template<class ReturnT, class WrapT>
    ReturnT* AllShardBatchIteratorBase<ReturnT, WrapT>::operator->()
    {
        return &wrapped->tuple;
    }

    template<class ReturnT, class WrapT>
    AllShardBatchIteratorBase<ReturnT, WrapT>&
        AllShardBatchIteratorBase<ReturnT, WrapT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    AllShardBatchIteratorBase<ReturnT, WrapT>
        AllShardBatchIteratorBase<ReturnT, WrapT>::operator++(int) const
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    AllShardBatchIteratorBase<ReturnT, WrapT>&
        AllShardBatchIteratorBase<ReturnT, WrapT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    AllShardBatchIteratorBase<ReturnT, WrapT>
        AllShardBatchIteratorBase<ReturnT, WrapT>::operator--(int) const
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicID AllShardBatchIteratorBase<ReturnT, WrapT>::ID() const
    {
        return wrapped->id;
    }
}