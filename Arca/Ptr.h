#pragma once

#include "RelicID.h"
#include "Handle.h"
#include "RelicTraits.h"
#include "Either.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    template<class T, class = void>
    class Ptr;

    template<class T>
    class Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>
    {
    public:
        using WrappedT = T;
    public:
        Ptr() = default;
        Ptr(RelicID id, Reliquary& owner);

        bool operator==(const Ptr& arg) const;
        bool operator!=(const Ptr& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        explicit operator T* () const;

        operator Ptr<const T>() const;

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
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::Ptr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class T>
    bool Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator==(const Ptr& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator!=(const Ptr& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator bool() const
    {
        if (id == 0)
            return false;

        return Get() != nullptr;
    }

    template<class T>
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator Handle() const
    {
        return Handle(ID(), Owner(), TypeFor<T>());
    }

    template<class T>
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator T* () const
    {
        return Get();
    }

    template<class T>
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator Ptr<const T>() const
    {
        return Ptr<const T>(id, *owner);
    }

    template<class T>
    T& Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator*() const
    {
        return *Get();
    }

    template<class T>
    T* Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator->() const
    {
        return Get();
    }

    template<class T>
    T* Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::Get() const
    {
        auto retrieved = Owner().FindStorage<T>(id);
        previousValue = retrieved;
        return retrieved;
    }

    template<class T>
    RelicID Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary& Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::Owner() const
    {
        return *owner;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Ptr<T, std::enable_if_t<!Arca::is_global_relic_v<T>>>, BinaryArchive>
        : public CompositeScribe<Arca::Ptr<T, std::enable_if_t<!Arca::is_global_relic_v<T>>>, BinaryArchive>
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

namespace Arca
{
    template<class T>
    class Ptr<T, std::enable_if_t<is_either_v<T>>>
    {
    public:
        using WrappedT = T;
        using ShardT = typename WrappedT::ShardT;
    private:
        using BareT = typename WrappedT::BareT;
    public:
        Ptr() = default;
        Ptr(RelicID id, Reliquary& owner);

        bool operator==(const Ptr& arg) const;
        bool operator!=(const Ptr& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        explicit operator ShardT* () const;

        operator Ptr<const T>() const;

        [[nodiscard]] ShardT& operator*() const;
        [[nodiscard]] ShardT* operator->() const;

        [[nodiscard]] ShardT* Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
        mutable ShardT* previousValue = nullptr;
    };

    template<class T>
    Ptr<T, std::enable_if_t<is_either_v<T>>>::Ptr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class T>
    bool Ptr<T, std::enable_if_t<is_either_v<T>>>::operator==(const Ptr& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Ptr<T, std::enable_if_t<is_either_v<T>>>::operator!=(const Ptr& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_either_v<T>>>::operator bool() const
    {
        if (id == 0)
            return false;

        return Get() != nullptr;
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_either_v<T>>>::operator Handle() const
    {
        return Handle(ID(), Owner(), TypeFor<T>());
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_either_v<T>>>::operator ShardT* () const
    {
        return Get();
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_either_v<T>>>::operator Ptr<const T>() const
    {
        return Ptr<const T>(id, *owner);
    }

    template<class T>
    auto Ptr<T, std::enable_if_t<is_either_v<T>>>::operator*() const -> ShardT&
    {
        return *Get();
    }

    template<class T>
    auto Ptr<T, std::enable_if_t<is_either_v<T>>>::operator->() const -> ShardT*
    {
        return Get();
    }

    template<class T>
    auto Ptr<T, std::enable_if_t<is_either_v<T>>>::Get() const -> ShardT*
    {
        const BareT* retrieved = Owner().template FindStorage<BareT>(id);
        if (!retrieved)
            retrieved = Owner().template FindStorage<const BareT>(id);
        previousValue = retrieved;
        return retrieved;
    }

    template<class T>
    RelicID Ptr<T, std::enable_if_t<is_either_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary& Ptr<T, std::enable_if_t<is_either_v<T>>>::Owner() const
    {
        return *owner;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Ptr<T, std::enable_if_t<Arca::is_either_v<T>>>, BinaryArchive>
        : public CompositeScribe<Arca::Ptr<T, std::enable_if_t<Arca::is_either_v<T>>>, BinaryArchive>
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

namespace Arca
{
    class Reliquary;

    template<class T>
    class Ptr<T, std::enable_if_t<is_global_relic_v<T>>>
    {
    public:
        using WrappedT = T;
    public:
        Ptr() = default;
        Ptr(RelicID id, Reliquary& owner);

        bool operator==(const Ptr& arg) const;
        bool operator!=(const Ptr& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        explicit operator T* () const;

        operator Ptr<const T>() const;

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
    Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::Ptr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class T>
    bool Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator==(const Ptr& arg) const
    {
        return owner == arg.owner;
    }

    template<class T>
    bool Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator!=(const Ptr& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator bool() const
    {
        if (id == 0)
            return false;

        return Get() != nullptr;
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator Handle() const
    {
        return Handle(id, Owner(), TypeFor<T>());
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator T* () const
    {
        return Get();
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator Ptr<const T>() const
    {
        return Ptr<const T>(id, *owner);
    }

    template<class T>
    T& Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator*() const
    {
        return *Get();
    }

    template<class T>
    T* Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator->() const
    {
        return Get();
    }

    template<class T>
    T* Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::Get() const
    {
        auto retrieved = Owner().template FindGlobalStorage<T>();
        previousValue = retrieved;
        return retrieved;
    }

    template<class T>
    RelicID Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary& Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::Owner() const
    {
        return *owner;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Ptr<T, std::enable_if_t<Arca::is_global_relic_v<T>>>, BinaryArchive>
        : public CompositeScribe<Arca::Ptr<T, std::enable_if_t<Arca::is_global_relic_v<T>>>, BinaryArchive>
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