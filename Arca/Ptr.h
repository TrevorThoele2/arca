#pragma once

#include "RelicID.h"
#include "Handle.h"
#include "RelicTraits.h"
#include "ShardTraits.h"
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
        Ptr(const Ptr& arg);
        Ptr(Ptr&& arg) noexcept;

        Ptr& operator=(const Ptr& arg);
        Ptr& operator=(Ptr&& arg) noexcept;

        bool operator==(const Ptr& arg) const;
        bool operator!=(const Ptr& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        operator T*() const;

        operator Ptr<const T>() const;

        [[nodiscard]] T& operator*() const;
        [[nodiscard]] T* operator->() const;

        [[nodiscard]] T* Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary* Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
        mutable T* value = nullptr;
    private:
        T* FindValueFromOwner();
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::Ptr(RelicID id, Reliquary& owner) :
        id(id), owner(&owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::Ptr(const Ptr& arg) :
        id(arg.id), owner(arg.owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::Ptr(Ptr&& arg) noexcept :
        id(arg.id), owner(arg.owner), value(arg.value)
    {}

    template<class T>
    auto Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator=(const Ptr& arg) -> Ptr&
    {
        id = arg.id;
        owner = arg.owner;
        value = FindValueFromOwner();
        return *this;
    }

    template<class T>
    auto Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator=(Ptr&& arg) noexcept -> Ptr&
    {
        id = arg.id;
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

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
        return value != nullptr;
    }

    template<class T>
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator Handle() const
    {
        return Handle(ID(), Owner(), TypeFor<T>(), HandleObjectTypeFor<T>());
    }

    template<class T>
    Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::operator T*() const
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
        return value;
    }

    template<class T>
    RelicID Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    T* Ptr<T, std::enable_if_t<!is_global_relic_v<T> && !is_either_v<T>>>::FindValueFromOwner()
    {
        if (owner == nullptr)
            return nullptr;
        return owner->FindStorage<T>(id);
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
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            if (archive.IsOutput())
            {
                auto id = object.ID();
                archive(id);
            }
            else
            {
                Arca::RelicID id;
                archive(id);
                object.id = id;

                auto reliquary = archive.template UserContext<Arca::Reliquary>();
                object.owner = reliquary;
            }
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
        Ptr(const Ptr& arg);
        Ptr(Ptr&& arg) noexcept;

        Ptr& operator=(const Ptr& arg);
        Ptr& operator=(Ptr&& arg) noexcept;

        bool operator==(const Ptr& arg) const;
        bool operator!=(const Ptr& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        operator ShardT*() const
        {
            return Get();
        }

        operator Ptr<const T>() const;

        [[nodiscard]] ShardT& operator*() const;
        [[nodiscard]] ShardT* operator->() const;

        [[nodiscard]] ShardT* Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary* Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
        mutable ShardT* value = nullptr;
    private:
        ShardT* FindValueFromOwner();
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    Ptr<T, std::enable_if_t<is_either_v<T>>>::Ptr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_either_v<T>>>::Ptr(const Ptr& arg) :
        id(arg.id), owner(arg.owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_either_v<T>>>::Ptr(Ptr&& arg) noexcept :
        id(arg.id), owner(arg.owner), value(arg.value)
    {}

    template<class T>
    auto Ptr<T, std::enable_if_t<is_either_v<T>>>::operator=(const Ptr& arg) -> Ptr&
    {
        id = arg.id;
        owner = arg.owner;
        value = FindValueFromOwner();
        return *this;
    }

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
        return value != nullptr;
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_either_v<T>>>::operator Handle() const
    {
        return Handle(ID(), Owner(), TypeFor<T>());
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
        return value;
    }

    template<class T>
    RelicID Ptr<T, std::enable_if_t<is_either_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* Ptr<T, std::enable_if_t<is_either_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto Ptr<T, std::enable_if_t<is_either_v<T>>>::FindValueFromOwner() -> ShardT*
    {
        if (owner == nullptr)
            return nullptr;
        return owner->template FindStorage<T>(id);
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
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.Get());
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
        Ptr(const Ptr& arg);
        Ptr(Ptr&& arg) noexcept;

        Ptr& operator=(const Ptr& arg);
        Ptr& operator=(Ptr&& arg);

        bool operator==(const Ptr& arg) const;
        bool operator!=(const Ptr& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        operator T*() const;

        operator Ptr<const T>() const;

        [[nodiscard]] T& operator*() const;
        [[nodiscard]] T* operator->() const;

        [[nodiscard]] T* Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary* Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
        mutable T* value = nullptr;
    private:
        T* FindValueFromOwner();
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::Ptr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::Ptr(const Ptr& arg) :
        id(arg.id), owner(arg.owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::Ptr(Ptr&& arg) noexcept :
        id(arg.id), owner(arg.owner), value(arg.value)
    {}

    template<class T>
    auto Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator=(const Ptr& arg) -> Ptr&
    {
        id = arg.id;
        owner = arg.owner;
        value = FindValueFromOwner();
        return *this;
    }

    template<class T>
    auto Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator=(Ptr&& arg) -> Ptr&
    {
        id = arg.id;
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

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
        return value != nullptr;
    }

    template<class T>
    Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::operator Handle() const
    {
        return Handle(id, Owner(), TypeFor<T>(), HandleObjectType::Relic);
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
        return value;
    }

    template<class T>
    RelicID Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    T* Ptr<T, std::enable_if_t<is_global_relic_v<T>>>::FindValueFromOwner()
    {
        if (owner == nullptr)
            return nullptr;
        return owner->template FindGlobalStorage<T>();
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
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}