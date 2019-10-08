#pragma once

#include "RelicTypeGraphNode.h"

namespace Arca
{
    template<class T>
    class RelicTypeGraphIteratorBase
    {
    public:
        explicit RelicTypeGraphIteratorBase(T* wrap);
        RelicTypeGraphIteratorBase(const RelicTypeGraphIteratorBase& arg);

        RelicTypeGraphIteratorBase& operator=(const RelicTypeGraphIteratorBase& arg);

        bool operator==(const RelicTypeGraphIteratorBase& arg) const;
        bool operator!=(const RelicTypeGraphIteratorBase& arg) const;

        T& operator*();
        T* operator->();

        RelicTypeGraphIteratorBase& operator++();
        RelicTypeGraphIteratorBase operator++(int) const;
    private:
        T* wrapped;
    private:
        T* Next() const;
    };

    template<class T>
    RelicTypeGraphIteratorBase<T>::RelicTypeGraphIteratorBase(T* wrap) : wrapped(wrap)
    {}

    template<class T>
    RelicTypeGraphIteratorBase<T>::RelicTypeGraphIteratorBase(const RelicTypeGraphIteratorBase& arg) :
        wrapped(arg.wrapped)
    {}

    template<class T>
    RelicTypeGraphIteratorBase<T>& RelicTypeGraphIteratorBase<T>::operator=(const RelicTypeGraphIteratorBase& arg)
    {
        wrapped = arg.wrapped;
        return *this;
    }

    template<class T>
    bool RelicTypeGraphIteratorBase<T>::operator==(const RelicTypeGraphIteratorBase& arg) const
    {
        return wrapped == arg.wrapped;
    }

    template<class T>
    bool RelicTypeGraphIteratorBase<T>::operator!=(const RelicTypeGraphIteratorBase& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    T& RelicTypeGraphIteratorBase<T>::operator*()
    {
        return wrapped->second;
    }

    template<class T>
    T* RelicTypeGraphIteratorBase<T>::operator->()
    {
        return &wrapped->second;
    }

    template<class T>
    RelicTypeGraphIteratorBase<T>& RelicTypeGraphIteratorBase<T>::operator++()
    {
        wrapped = Next();
        return *this;
    }

    template<class T>
    RelicTypeGraphIteratorBase<T> RelicTypeGraphIteratorBase<T>::operator++(int) const
    {
        auto copy = *this;
        wrapped = Next();
        return copy;
    }

    template<class T>
    T* RelicTypeGraphIteratorBase<T>::Next() const
    {
        auto parent = wrapped->parent;
        if (!parent)
            return nullptr;

        auto found = std::find_if(
            parent->children.begin(),
            parent->children.end(),
            [this](const RelicTypeGraphNode::NodePtr& arg)
        {
            return arg.get() == wrapped;
        });

        auto next = std::next(found);
        if (next == parent->children.end())
            return parent;

        auto child = (*next)->children.begin();
        while (child != (*next)->children.end())
        {
            next = child;
            child = (*next)->children.begin();
        }

        return next->get();
    }
}