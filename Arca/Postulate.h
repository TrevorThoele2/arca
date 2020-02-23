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
        using StoredT = std::optional<std::remove_const_t<T>>;
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
        static StoredT FindValueFrom(Reliquary* reliquary);
        StoredT FindValueFromOwner() const;
        bool IsSetup() const;
        constexpr static StoredT EmptyValue();
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    class Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>
    {
    private:
        using StoredT = std::remove_const_t<T>;
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
        static StoredT FindValueFrom(Reliquary* reliquary);
        StoredT FindValueFromOwner() const;
        bool IsSetup() const;
        constexpr static StoredT EmptyValue();
    private:
        INSCRIPTION_ACCESS;
    };
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