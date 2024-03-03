#pragma once

#include "RelicID.h"
#include "Handle.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    template<class T>
    class Global
    {
    public:
        using WrappedT = T;
    public:
        Global() = default;
        Global(RelicID id, Reliquary& owner);

        bool operator==(const Global& arg) const;
        bool operator!=(const Global& arg) const;

        explicit operator bool() const;

        operator Handle() const;

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
    Global<T>::Global(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class T>
    bool Global<T>::operator==(const Global& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Global<T>::operator!=(const Global& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Global<T>::operator bool() const
    {
        if (id == 0)
            return false;

        return Get() != nullptr;
    }

    template<class T>
    Global<T>::operator Handle() const
    {
        return Handle(ID(), Owner());
    }

    template<class T>
    Global<T>::operator T* ()
    {
        return Get();
    }

    template<class T>
    Global<T>::operator const T* () const
    {
        return Get();
    }

    template<class T>
    T& Global<T>::operator*()
    {
        return *Get();
    }

    template<class T>
    const T& Global<T>::operator*() const
    {
        return *Get();
    }

    template<class T>
    T* Global<T>::operator->()
    {
        return Get();
    }

    template<class T>
    const T* Global<T>::operator->() const
    {
        return Get();
    }

    template<class T>
    T* Global<T>::Get()
    {
        return Owner().FindGlobalStorage<T>(id);
    }

    template<class T>
    const T* Global<T>::Get() const
    {
        return Owner().FindGlobalStorage<T>(id);
    }

    template<class T>
    RelicID Global<T>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary& Global<T>::Owner() const
    {
        return *owner;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Global<T>, BinaryArchive>
        : public CompositeScribe<Arca::Global<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::Global<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive)
        {

        }
    };
}