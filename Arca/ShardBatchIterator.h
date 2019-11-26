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
        template<class U, std::enable_if_t<std::is_const_v<U>, int> = 0>
        void ForwardWrapped();
        template<class U, std::enable_if_t<!std::is_const_v<U>, int> = 0>
        void ForwardWrapped();
        template<class U, std::enable_if_t<std::is_const_v<U>, int> = 0>
        void BackwardWrapped();
        template<class U, std::enable_if_t<!std::is_const_v<U>, int> = 0>
        void BackwardWrapped();
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
    {
        if (wrapped != end && !std::is_const_v<ReturnT> && wrapped->isConst)
            ForwardWrapped<ReturnT>();
    }

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
        return wrapped->shard;
    }

    template<class ReturnT, class WrapT>
    ReturnT* ShardBatchIteratorBase<ReturnT, WrapT>::operator->()
    {
        return &wrapped->shard;
    }

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>&
        ShardBatchIteratorBase<ReturnT, WrapT>::operator++()
    {
        ForwardWrapped<ReturnT>();
        return *this;
    }

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>
        ShardBatchIteratorBase<ReturnT, WrapT>::operator++(int) const
    {
        auto copy = *this;
        ForwardWrapped<ReturnT>();
        return copy;
    }

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>&
        ShardBatchIteratorBase<ReturnT, WrapT>::operator--()
    {
        BackwardWrapped<ReturnT>();
        return *this;
    }

    template<class ReturnT, class WrapT>
    ShardBatchIteratorBase<ReturnT, WrapT>
        ShardBatchIteratorBase<ReturnT, WrapT>::operator--(int) const
    {
        auto copy = *this;
        BackwardWrapped<ReturnT>();
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicID ShardBatchIteratorBase<ReturnT, WrapT>::ID() const
    {
        return wrapped->id;
    }

    template<class ReturnT, class WrapT>
    template<class U, std::enable_if_t<std::is_const_v<U>, int>>
    void ShardBatchIteratorBase<ReturnT, WrapT>::ForwardWrapped()
    {
        ++wrapped;
        while (wrapped != end && !wrapped->isConst)
            ++wrapped;
    }

    template<class ReturnT, class WrapT>
    template<class U, std::enable_if_t<!std::is_const_v<U>, int>>
    void ShardBatchIteratorBase<ReturnT, WrapT>::ForwardWrapped()
    {
        ++wrapped;
    }
    
    template<class ReturnT, class WrapT>
    template<class U, std::enable_if_t<std::is_const_v<U>, int>>
    void ShardBatchIteratorBase<ReturnT, WrapT>::BackwardWrapped()
    {
        --wrapped;
        while (wrapped != end && !wrapped->isConst)
            --wrapped;
    }

    template<class ReturnT, class WrapT>
    template<class U, std::enable_if_t<!std::is_const_v<U>, int>>
    void ShardBatchIteratorBase<ReturnT, WrapT>::BackwardWrapped()
    {
        --wrapped;
    }
}