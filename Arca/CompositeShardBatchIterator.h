#pragma once

#include "RelicID.h"

namespace Arca
{
    template<class ReturnT, class WrapT>
    class CompositeShardBatchIteratorBase
    {
    public:
        explicit CompositeShardBatchIteratorBase(const WrapT& wrap, const WrapT& end);
        CompositeShardBatchIteratorBase(const CompositeShardBatchIteratorBase& arg);

        CompositeShardBatchIteratorBase& operator=(const CompositeShardBatchIteratorBase& arg);

        bool operator==(const CompositeShardBatchIteratorBase& arg) const;
        bool operator!=(const CompositeShardBatchIteratorBase& arg) const;

        ReturnT& operator*();
        ReturnT* operator->();

        CompositeShardBatchIteratorBase& operator++();
        CompositeShardBatchIteratorBase operator++(int) const;
        CompositeShardBatchIteratorBase& operator--();
        CompositeShardBatchIteratorBase operator--(int) const;

        [[nodiscard]] RelicID ID() const;
    private:
        WrapT wrapped;
        WrapT end;
    private:
        template<class U>
        friend class ShardBatch;
    };

    template<class ReturnT, class WrapT>
    CompositeShardBatchIteratorBase<ReturnT, WrapT>::CompositeShardBatchIteratorBase(
        const WrapT& wrap, const WrapT& end)
        :
        wrapped(wrap),
        end(end)
    {}

    template<class ReturnT, class WrapT>
    CompositeShardBatchIteratorBase<ReturnT, WrapT>::CompositeShardBatchIteratorBase(const CompositeShardBatchIteratorBase& arg) :
        wrapped(arg.wrapped)
    {}

    template<class ReturnT, class WrapT>
    CompositeShardBatchIteratorBase<ReturnT, WrapT>&
        CompositeShardBatchIteratorBase<ReturnT, WrapT>::operator=(const CompositeShardBatchIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    bool CompositeShardBatchIteratorBase<ReturnT, WrapT>::operator==(const CompositeShardBatchIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class ReturnT, class WrapT>
    bool CompositeShardBatchIteratorBase<ReturnT, WrapT>::operator!=(const CompositeShardBatchIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class ReturnT, class WrapT>
    ReturnT& CompositeShardBatchIteratorBase<ReturnT, WrapT>::operator*()
    {
        return wrapped->tuple;
    }

    template<class ReturnT, class WrapT>
    ReturnT* CompositeShardBatchIteratorBase<ReturnT, WrapT>::operator->()
    {
        return &wrapped->tuple;
    }

    template<class ReturnT, class WrapT>
    CompositeShardBatchIteratorBase<ReturnT, WrapT>&
        CompositeShardBatchIteratorBase<ReturnT, WrapT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    CompositeShardBatchIteratorBase<ReturnT, WrapT>
        CompositeShardBatchIteratorBase<ReturnT, WrapT>::operator++(int) const
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    CompositeShardBatchIteratorBase<ReturnT, WrapT>&
        CompositeShardBatchIteratorBase<ReturnT, WrapT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    CompositeShardBatchIteratorBase<ReturnT, WrapT>
        CompositeShardBatchIteratorBase<ReturnT, WrapT>::operator--(int) const
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicID CompositeShardBatchIteratorBase<ReturnT, WrapT>::ID() const
    {
        return wrapped->id;
    }
}