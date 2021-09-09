#pragma once

#include "RelicID.h"

namespace Arca
{
    template<class ReturnT, class WrapT>
    class ShardBatchIteratorBase
    {
    public:
        explicit ShardBatchIteratorBase(const WrapT& wrap, const WrapT& end);
        ShardBatchIteratorBase(const ShardBatchIteratorBase& arg);

        ShardBatchIteratorBase& operator=(const ShardBatchIteratorBase& arg);

        bool operator==(const ShardBatchIteratorBase& arg) const;
        bool operator!=(const ShardBatchIteratorBase& arg) const;

        ReturnT& operator*();
        ReturnT* operator->();

        ShardBatchIteratorBase& operator++();
        ShardBatchIteratorBase operator++(int) const;
        ShardBatchIteratorBase& operator--();
        ShardBatchIteratorBase operator--(int) const;

        [[nodiscard]] RelicID ID() const;
    private:
        WrapT wrapped;
        WrapT end;
    private:
        template<class U>
        friend class ShardBatch;
    };

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>::ShardBatchIteratorBase(
        const WrapT& wrap, const WrapT& end)
        :
        wrapped(wrap),
        end(end)
    {}

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>::ShardBatchIteratorBase(const ShardBatchIteratorBase& arg) :
        wrapped(arg.wrapped)
    {}

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>&
        ShardBatchIteratorBase<ReturnT, WrapT>::operator=(const ShardBatchIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    bool ShardBatchIteratorBase<ReturnT, WrapT>::operator==(const ShardBatchIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class ReturnT, class WrapT>
    bool ShardBatchIteratorBase<ReturnT, WrapT>::operator!=(const ShardBatchIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class ReturnT, class WrapT>
    ReturnT& ShardBatchIteratorBase<ReturnT, WrapT>::operator*()
    {
        return *wrapped->second.shard;
    }

    template<class ReturnT, class WrapT>
    ReturnT* ShardBatchIteratorBase<ReturnT, WrapT>::operator->()
    {
        return wrapped->second.shard.get();
    }

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>&
        ShardBatchIteratorBase<ReturnT, WrapT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>
        ShardBatchIteratorBase<ReturnT, WrapT>::operator++(int) const
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>&
        ShardBatchIteratorBase<ReturnT, WrapT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>
        ShardBatchIteratorBase<ReturnT, WrapT>::operator--(int) const
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicID ShardBatchIteratorBase<ReturnT, WrapT>::ID() const
    {
        return wrapped->first;
    }
}