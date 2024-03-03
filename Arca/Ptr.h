#pragma once

#include "RelicID.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    template<class T>
    class Ptr
    {
    public:
        Ptr();
        Ptr(RelicID id, Reliquary& owner);

        T* operator*();
        const T* operator*() const;
        T& operator->();
        const T& operator->() const;

        T* Get();
        const T* Get() const;
    private:
        RelicID id;
        Reliquary* owner;
    };

    template<class T>
    Ptr<T>::Ptr() : id(0), owner(nullptr)
    {}

    template<class T>
    Ptr<T>::Ptr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class T>
    T* Ptr<T>::operator*()
    {
        return Get();
    }

    template<class T>
    const T* Ptr<T>::operator*() const
    {
        return Get();
    }

    template<class T>
    T& Ptr<T>::operator->()
    {
        return *Get();
    }

    template<class T>
    const T& Ptr<T>::operator->() const
    {
        return *Get();
    }

    template<class T>
    T* Ptr<T>::Get()
    {

    }

    template<class T>
    const T* Ptr<T>::Get() const
    {
        
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Ptr<T>, BinaryArchive>
        : public CompositeScribe<Arca::Ptr<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::Ptr<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive)
        {
            
        }
    };
}