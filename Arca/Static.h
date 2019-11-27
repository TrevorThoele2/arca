#pragma once

#include "RelicID.h"
#include "RelicHandle.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    template<class T>
    class Static
    {
    public:
        using WrappedT = T;
    public:
        Static() = default;
        Static(RelicID id, Reliquary& owner);

        bool operator==(const Static& arg) const;
        bool operator!=(const Static& arg) const;

        explicit operator bool() const;

        operator RelicHandle() const;

        explicit operator T* ();
        explicit operator const T* () const;

        [[nodiscard]] T& operator*();
        [[nodiscard]] const T& operator*() const;
        [[nodiscard]] T* operator->();
        [[nodiscard]] const T* operator->() const;

        [[nodiscard]] T* Get();
        [[nodiscard]] const T* Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    };

    template<class T>
    Static<T>::Static(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class T>
    bool Static<T>::operator==(const Static& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Static<T>::operator!=(const Static& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Static<T>::operator bool() const
    {
        if (id == 0)
            return false;

        return Get() != nullptr;
    }

    template<class T>
    Static<T>::operator RelicHandle() const
    {
        return RelicHandle(ID(), Owner());
    }

    template<class T>
    Static<T>::operator T* ()
    {
        return Get();
    }

    template<class T>
    Static<T>::operator const T* () const
    {
        return Get();
    }

    template<class T>
    T& Static<T>::operator*()
    {
        return *Get();
    }

    template<class T>
    const T& Static<T>::operator*() const
    {
        return *Get();
    }

    template<class T>
    T* Static<T>::operator->()
    {
        return Get();
    }

    template<class T>
    const T* Static<T>::operator->() const
    {
        return Get();
    }

    template<class T>
    T* Static<T>::Get()
    {
        return Owner().FindStaticStorage<T>(id);
    }

    template<class T>
    const T* Static<T>::Get() const
    {
        return Owner().FindStaticStorage<T>(id);
    }

    template<class T>
    RelicID Static<T>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary& Static<T>::Owner() const
    {
        return *owner;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Static<T>, BinaryArchive>
        : public CompositeScribe<Arca::Static<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::Static<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive)
        {

        }
    };
}