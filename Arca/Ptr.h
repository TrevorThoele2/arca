#pragma once

#include "RelicID.h"
#include "RelicHandle.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    template<class T>
    class Ptr
    {
    public:
        using WrappedT = T;
    public:
        Ptr() = default;
        Ptr(RelicID id, Reliquary& owner);

        bool operator==(const Ptr& arg) const;
        bool operator!=(const Ptr& arg) const;

        explicit operator bool() const;

        operator RelicHandle() const;

        explicit operator T* () const;

        [[nodiscard]] T& operator*() const;
        [[nodiscard]] T* operator->() const;

        [[nodiscard]] T* Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
        mutable T* previousValue = nullptr;
    };

    template<class T>
    Ptr<T>::Ptr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class T>
    bool Ptr<T>::operator==(const Ptr& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Ptr<T>::operator!=(const Ptr& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Ptr<T>::operator bool() const
    {
        if (id == 0)
            return false;

        return Get() != nullptr;
    }

    template<class T>
    Ptr<T>::operator RelicHandle() const
    {
        return RelicHandle(ID(), Owner());
    }

    template<class T>
    Ptr<T>::operator T* () const
    {
        return Get();
    }

    template<class T>
    T& Ptr<T>::operator*() const
    {
        return *Get();
    }

    template<class T>
    T* Ptr<T>::operator->() const
    {
        return Get();
    }

    template<class T>
    T* Ptr<T>::Get() const
    {
        auto retrieved = Owner().FindStorage<T>(id);
        previousValue = retrieved;
        return retrieved;
    }

    template<class T>
    RelicID Ptr<T>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary& Ptr<T>::Owner() const
    {
        return *owner;
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