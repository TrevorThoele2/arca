#pragma once

#include "RelicID.h"

namespace Arca
{
    template<class ReturnT, class WrapT>
    class RelicMapIteratorBase
    {
    public:
        explicit RelicMapIteratorBase(const WrapT& wrap);
        RelicMapIteratorBase(const RelicMapIteratorBase& arg);

        RelicMapIteratorBase& operator=(const RelicMapIteratorBase& arg);

        bool operator==(const RelicMapIteratorBase& arg) const;
        bool operator!=(const RelicMapIteratorBase& arg) const;

        ReturnT& operator*();
        ReturnT* operator->();

        RelicMapIteratorBase& operator++();
        RelicMapIteratorBase operator++(int) const;
        RelicMapIteratorBase& operator--();
        RelicMapIteratorBase operator--(int) const;

        [[nodiscard]] RelicID ID() const;
    private:
        WrapT wrapped;
    private:
        template<class U>
        friend class RelicMap;
    };

    template<class ReturnT, class WrapT>
    RelicMapIteratorBase<ReturnT, WrapT>::RelicMapIteratorBase(const WrapT& wrap) : wrapped(wrap)
    {}

    template<class ReturnT, class WrapT>
    RelicMapIteratorBase<ReturnT, WrapT>::RelicMapIteratorBase(const RelicMapIteratorBase& arg) : wrapped(arg.wrapped)
    {}

    template<class ReturnT, class WrapT>
    RelicMapIteratorBase<ReturnT, WrapT>& RelicMapIteratorBase<ReturnT, WrapT>::operator=(
        const RelicMapIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    bool RelicMapIteratorBase<ReturnT, WrapT>::operator==(const RelicMapIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class ReturnT, class WrapT>
    bool RelicMapIteratorBase<ReturnT, WrapT>::operator!=(const RelicMapIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class ReturnT, class WrapT>
    ReturnT& RelicMapIteratorBase<ReturnT, WrapT>::operator*()
    {
        return wrapped->second;
    }

    template<class ReturnT, class WrapT>
    ReturnT* RelicMapIteratorBase<ReturnT, WrapT>::operator->()
    {
        return &wrapped->second;
    }

    template<class ReturnT, class WrapT>
    RelicMapIteratorBase<ReturnT, WrapT>& RelicMapIteratorBase<ReturnT, WrapT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    RelicMapIteratorBase<ReturnT, WrapT> RelicMapIteratorBase<ReturnT, WrapT>::operator++(int) const
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicMapIteratorBase<ReturnT, WrapT>& RelicMapIteratorBase<ReturnT, WrapT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class ReturnT, class WrapT>
    RelicMapIteratorBase<ReturnT, WrapT> RelicMapIteratorBase<ReturnT, WrapT>::operator--(int) const
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class ReturnT, class WrapT>
    RelicID RelicMapIteratorBase<ReturnT, WrapT>::ID() const
    {
        return wrapped.first;
    }
}