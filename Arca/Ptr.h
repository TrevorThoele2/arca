#pragma once

#include "RelicID.h"
#include "Handle.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    template<class T>
    class LocalPtr
    {
    public:
        using WrappedT = T;
    public:
        LocalPtr() = default;
        LocalPtr(RelicID id, Reliquary& owner);

        bool operator==(const LocalPtr& arg) const;
        bool operator!=(const LocalPtr& arg) const;

        explicit operator bool() const;

        operator Handle() const;

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
    LocalPtr<T>::LocalPtr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class T>
    bool LocalPtr<T>::operator==(const LocalPtr& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool LocalPtr<T>::operator!=(const LocalPtr& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    LocalPtr<T>::operator bool() const
    {
        if (id == 0)
            return false;

        return Get() != nullptr;
    }

    template<class T>
    LocalPtr<T>::operator Handle() const
    {
        return Handle(ID(), Owner());
    }

    template<class T>
    LocalPtr<T>::operator T* () const
    {
        return Get();
    }

    template<class T>
    T& LocalPtr<T>::operator*() const
    {
        return *Get();
    }

    template<class T>
    T* LocalPtr<T>::operator->() const
    {
        return Get();
    }

    template<class T>
    T* LocalPtr<T>::Get() const
    {
        auto retrieved = Owner().FindStorage<T>(id);
        previousValue = retrieved;
        return retrieved;
    }

    template<class T>
    RelicID LocalPtr<T>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary& LocalPtr<T>::Owner() const
    {
        return *owner;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::LocalPtr<T>, BinaryArchive>
        : public CompositeScribe<Arca::LocalPtr<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::LocalPtr<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive)
        {
            
        }
    };
}

namespace Arca
{
    class Reliquary;

    template<class T>
    class GlobalPtr
    {
    public:
        using WrappedT = T;
    public:
        GlobalPtr() = default;
        GlobalPtr(RelicID id, Reliquary& owner);

        bool operator==(const GlobalPtr& arg) const;
        bool operator!=(const GlobalPtr& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        explicit operator T* () const;

        [[nodiscard]] T& operator*() const;
        [[nodiscard]] T* operator->() const;

        [[nodiscard]] T* Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    private:
        RelicID id;
        Reliquary* owner = nullptr;
        mutable T* previousValue = nullptr;
    };

    template<class T>
    GlobalPtr<T>::GlobalPtr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class T>
    bool GlobalPtr<T>::operator==(const GlobalPtr& arg) const
    {
        return owner == arg.owner;
    }

    template<class T>
    bool GlobalPtr<T>::operator!=(const GlobalPtr& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    GlobalPtr<T>::operator bool() const
    {
        return Get() != nullptr;
    }

    template<class T>
    GlobalPtr<T>::operator Handle() const
    {
        return Handle(id, Owner());
    }

    template<class T>
    GlobalPtr<T>::operator T* () const
    {
        return Get();
    }

    template<class T>
    T& GlobalPtr<T>::operator*() const
    {
        return *Get();
    }

    template<class T>
    T* GlobalPtr<T>::operator->() const
    {
        return Get();
    }

    template<class T>
    T* GlobalPtr<T>::Get() const
    {
        auto retrieved = Owner().template FindGlobalStorage<T>();
        previousValue = retrieved;
        return retrieved;
    }

    template<class T>
    RelicID GlobalPtr<T>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary& GlobalPtr<T>::Owner() const
    {
        return *owner;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::GlobalPtr<T>, BinaryArchive>
        : public CompositeScribe<Arca::GlobalPtr<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::GlobalPtr<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive)
        {

        }
    };
}