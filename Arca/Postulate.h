#pragma once

#include "ReferenceTypeFor.h"
#include "UsableForPostulate.h"

#include "Serialization.h"

namespace Arca
{
    template<class T, class Enable = void>
    class Postulate;

    template<class T>
    class Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>
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

        operator Postulate<const T>() const;

        [[nodiscard]] ValueT& operator*() const;

        [[nodiscard]] ValueT& Get() const;

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
    Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::Postulate(Reliquary& owner) : owner(&owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::Postulate(const Postulate& arg) : owner(arg.owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::Postulate(Postulate&& arg) noexcept :
        owner(arg.owner), value(arg.value)
    {}

    template<class T>
    auto Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::operator=(const Postulate& arg) -> Postulate&
    {
        owner = arg.owner;
        value = FindValueFromOwner();
        return *this;
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::operator=(Postulate&& arg) noexcept -> Postulate&
    {
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

    template<class T>
    bool Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::operator==(const Postulate& arg) const
    {
        return owner == arg.owner;
    }

    template<class T>
    bool Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::operator!=(const Postulate& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::operator bool() const
    {
        return IsSetup();
    }

    template<class T>
    Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::operator Postulate<const T>() const
    {
        return Postulate<const T>(*owner);
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::operator*() const -> ValueT&
    {
        return Get();
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::Get() const -> ValueT&
    {
        if (!IsSetup())
            value = FindValueFromOwner();

        return *value;
    }

    template<class T>
    Reliquary* Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::FindValueFromOwner() const -> StoredT
    {
        if (owner == nullptr)
            return EmptyValue();

        return owner->template FindPostulateValue<T>();
    }

    template<class T>
    bool Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::IsSetup() const
    {
        return static_cast<bool>(value);
    }

    template<class T>
    constexpr auto Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::EmptyValue() -> StoredT
    {
        return {};
    }

    template<class T>
    class Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>
    {
    private:
        using StoredT = T;
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

        operator Postulate<const T>() const;

        [[nodiscard]] std::remove_pointer_t<ValueT>& operator*() const;
        [[nodiscard]] ValueT operator->() const;

        [[nodiscard]] ValueT Get() const;

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
    Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::Postulate(Reliquary& owner) : owner(&owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::Postulate(const Postulate& arg) : owner(arg.owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::Postulate(Postulate&& arg) noexcept :
        owner(arg.owner), value(arg.value)
    {}

    template<class T>
    auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::operator=(const Postulate& arg) -> Postulate&
    {
        owner = arg.owner;
        value = FindValueFromOwner();
        return *this;
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::operator=(Postulate&& arg) noexcept -> Postulate&
    {
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

    template<class T>
    bool Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::operator==(const Postulate& arg) const
    {
        return owner == arg.owner;
    }

    template<class T>
    bool Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::operator!=(const Postulate& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::operator bool() const
    {
        return IsSetup();
    }

    template<class T>
    Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::operator Postulate<const T>() const
    {
        return Postulate<const T>(*owner);
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::operator*() const -> std::remove_pointer_t<ValueT>&
    {
        return *Get();
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::operator->() const -> ValueT
    {
        return Get();
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::Get() const -> ValueT
    {
        if (!IsSetup())
            value = FindValueFromOwner();

        return value;
    }

    template<class T>
    Reliquary* Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::FindValueFromOwner() const -> StoredT
    {
        if (owner == nullptr)
            return EmptyValue();

        return owner->template FindPostulateValue<T>();
    }

    template<class T>
    bool Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::IsSetup() const
    {
        return static_cast<bool>(value);
    }

    template<class T>
    constexpr auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::EmptyValue() -> StoredT
    {
        return nullptr;
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