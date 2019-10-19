#pragma once

namespace Arca
{
    template<class T, class WrappedIteratorT>
    class IDManagerIteratorBase
    {
    public:
        using IdentifierT = typename WrappedIteratorT::value_type::IdentifierT;
    public:
        IDManagerIteratorBase();
        IDManagerIteratorBase(const WrappedIteratorT& wrapped);

        bool operator==(const IDManagerIteratorBase& arg) const;
        bool operator!=(const IDManagerIteratorBase& arg) const;

        T& operator*();
        T* operator->();

        IDManagerIteratorBase& operator++();
        IDManagerIteratorBase operator++(int);
        IDManagerIteratorBase& operator--();
        IDManagerIteratorBase operator--(int);

        IdentifierT ID() const;
    private:
        WrappedIteratorT wrapped;
    private:
        template<class ID, class U>
        friend class IDManager;
    };

    template<class T, class WrappedIteratorT>
    IDManagerIteratorBase<T, WrappedIteratorT>::IDManagerIteratorBase()
    {}

    template<class T, class WrappedIteratorT>
    IDManagerIteratorBase<T, WrappedIteratorT>::IDManagerIteratorBase(const WrappedIteratorT& wrapped) : wrapped(wrapped)
    {}

    template<class T, class WrappedIteratorT>
    bool IDManagerIteratorBase<T, WrappedIteratorT>::operator==(const IDManagerIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class T, class WrappedIteratorT>
    bool IDManagerIteratorBase<T, WrappedIteratorT>::operator!=(const IDManagerIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class T, class WrappedIteratorT>
    T& IDManagerIteratorBase<T, WrappedIteratorT>::operator*()
    {
        return wrapped->value;
    }

    template<class T, class WrappedIteratorT>
    T* IDManagerIteratorBase<T, WrappedIteratorT>::operator->()
    {
        return &wrapped->value;
    }

    template<class T, class WrappedIteratorT>
    IDManagerIteratorBase<T, WrappedIteratorT>& IDManagerIteratorBase<T, WrappedIteratorT>::operator++()
    {
        ++wrapped;
        return *this;
    }

    template<class T, class WrappedIteratorT>
    IDManagerIteratorBase<T, WrappedIteratorT> IDManagerIteratorBase<T, WrappedIteratorT>::operator++(int)
    {
        auto copy = *this;
        ++wrapped;
        return copy;
    }

    template<class T, class WrappedIteratorT>
    IDManagerIteratorBase<T, WrappedIteratorT>& IDManagerIteratorBase<T, WrappedIteratorT>::operator--()
    {
        --wrapped;
        return *this;
    }

    template<class T, class WrappedIteratorT>
    IDManagerIteratorBase<T, WrappedIteratorT> IDManagerIteratorBase<T, WrappedIteratorT>::operator--(int)
    {
        auto copy = *this;
        --wrapped;
        return copy;
    }

    template<class T, class WrappedIteratorT>
    auto IDManagerIteratorBase<T, WrappedIteratorT>::ID() const -> IdentifierT
    {
        return wrapped->id;
    }
}