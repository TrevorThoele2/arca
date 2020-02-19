#pragma once

#include "ReferenceTypeFor.h"
#include "UsableForPostulate.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Postulate
    {
    private:
        using StoredT = std::optional<T>;
    public:
        using ValueT = T;
    public:
        Postulate() = default;
        explicit Postulate(Reliquary& owner);
        Postulate(const Postulate& arg);
        Postulate(Postulate&& arg) noexcept;

        Postulate& operator=(const Postulate& arg);
        Postulate& operator=(Postulate&& arg) noexcept;

        bool operator==(const Postulate& arg) const;
        bool operator!=(const Postulate& arg) const;

        explicit operator bool() const;

        operator ValueT* () const
        {
            return Get();
        }

        operator Postulate<const T>() const;

        [[nodiscard]] ValueT& operator*() const;
        [[nodiscard]] ValueT* operator->() const;

        [[nodiscard]] ValueT* Get() const;

        [[nodiscard]] Reliquary* Owner() const;
    private:
        Reliquary* owner = nullptr;
        mutable StoredT value = EmptyValue();
    private:
        StoredT FindValueFromOwner() const;
        bool IsSetup() const;
        constexpr static StoredT EmptyValue();
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    Postulate<T>::Postulate(Reliquary& owner) : owner(&owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Postulate<T>::Postulate(const Postulate& arg) : owner(arg.owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Postulate<T>::Postulate(Postulate&& arg) noexcept :
        owner(arg.owner), value(arg.value)
    {}

    template<class T>
    auto Postulate<T>::operator=(const Postulate& arg) -> Postulate&
    {
        owner = arg.owner;
        value = FindValueFromOwner();
        return *this;
    }

    template<class T>
    auto Postulate<T>::operator=(Postulate&& arg) noexcept -> Postulate&
    {
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

    template<class T>
    bool Postulate<T>::operator==(const Postulate& arg) const
    {
        return owner == arg.owner;
    }

    template<class T>
    bool Postulate<T>::operator!=(const Postulate& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Postulate<T>::operator bool() const
    {
        return IsSetup();
    }

    template<class T>
    Postulate<T>::operator Postulate<const T>() const
    {
        return Postulate<const T>(*owner);
    }

    template<class T>
    auto Postulate<T>::operator*() const -> ValueT&
    {
        return *Get();
    }

    template<class T>
    auto Postulate<T>::operator->() const -> ValueT*
    {
        return Get();
    }

    template<class T>
    auto Postulate<T>::Get() const -> ValueT*
    {
        if (!IsSetup())
            value = FindValueFromOwner();

        return value ? &*value : nullptr;
    }

    template<class T>
    Reliquary* Postulate<T>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto Postulate<T>::FindValueFromOwner() const -> StoredT
    {
        if (owner == nullptr)
            return EmptyValue();

        return owner->template FindPostulate<T>();
    }

    template<class T>
    bool Postulate<T>::IsSetup() const
    {
        return static_cast<bool>(value);
    }

    template<class T>
    constexpr auto Postulate<T>::EmptyValue() -> StoredT
    {
        return {};
    }

    template<class T>
    struct ReferenceTypeFor<T, std::enable_if_t<usable_for_postulate_v<T>>>
    {
        using Type = Postulate<T>;
    };

    template<class T, std::enable_if_t<usable_for_postulate_v<T>, int> = 0>
    Postulate<T> ToReference(RelicID id, Reliquary& owner)
    {
        return Postulate<T>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Postulate<T>, BinaryArchive>
        : public CompositeScribe<Arca::Postulate<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::Postulate<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT&, ArchiveT&) override
        {}
    };
}