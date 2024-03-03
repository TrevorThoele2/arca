#pragma once

#include "IndexTypeFor.h"
#include "UsableForComputedIndex.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class ComputedIndex
    {
    private:
        using StoredT = std::optional<T>;
    public:
        using ValueT = T;
    public:
        ComputedIndex() = default;
        explicit ComputedIndex(Reliquary& owner);
        ComputedIndex(const ComputedIndex& arg);
        ComputedIndex(ComputedIndex&& arg) noexcept;

        ComputedIndex& operator=(const ComputedIndex& arg);
        ComputedIndex& operator=(ComputedIndex&& arg) noexcept;

        bool operator==(const ComputedIndex& arg) const;
        bool operator!=(const ComputedIndex& arg) const;

        explicit operator bool() const;

        operator ValueT* () const
        {
            return Get();
        }

        operator ComputedIndex<const T>() const;

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
    ComputedIndex<T>::ComputedIndex(Reliquary& owner) : owner(&owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    ComputedIndex<T>::ComputedIndex(const ComputedIndex& arg) : owner(arg.owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    ComputedIndex<T>::ComputedIndex(ComputedIndex&& arg) noexcept :
        owner(arg.owner), value(arg.value)
    {}

    template<class T>
    auto ComputedIndex<T>::operator=(const ComputedIndex& arg) -> ComputedIndex&
    {
        owner = arg.owner;
        value = FindValueFromOwner();
        return *this;
    }

    template<class T>
    auto ComputedIndex<T>::operator=(ComputedIndex&& arg) noexcept -> ComputedIndex&
    {
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

    template<class T>
    bool ComputedIndex<T>::operator==(const ComputedIndex& arg) const
    {
        return owner == arg.owner;
    }

    template<class T>
    bool ComputedIndex<T>::operator!=(const ComputedIndex& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    ComputedIndex<T>::operator bool() const
    {
        return IsSetup();
    }

    template<class T>
    ComputedIndex<T>::operator ComputedIndex<const T>() const
    {
        return ComputedIndex<const T>(*owner);
    }

    template<class T>
    auto ComputedIndex<T>::operator*() const -> ValueT&
    {
        return *Get();
    }

    template<class T>
    auto ComputedIndex<T>::operator->() const -> ValueT*
    {
        return Get();
    }

    template<class T>
    auto ComputedIndex<T>::Get() const -> ValueT*
    {
        if (!IsSetup())
            value = FindValueFromOwner();

        return value ? &*value : nullptr;
    }

    template<class T>
    Reliquary* ComputedIndex<T>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto ComputedIndex<T>::FindValueFromOwner() const -> StoredT
    {
        if (owner == nullptr)
            return EmptyValue();

        return owner->template FindGlobalComputation<T>();
    }

    template<class T>
    bool ComputedIndex<T>::IsSetup() const
    {
        return static_cast<bool>(value);
    }

    template<class T>
    constexpr auto ComputedIndex<T>::EmptyValue() -> StoredT
    {
        return {};
    }

    template<class T>
    struct IndexTypeFor<T, std::enable_if_t<usable_for_computed_index_v<T>>>
    {
        using Type = ComputedIndex<T>;
    };

    template<class T, std::enable_if_t<usable_for_computed_index_v<T>, int> = 0>
    ComputedIndex<T> ToIndex(RelicID id, Reliquary& owner)
    {
        return ComputedIndex<T>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::ComputedIndex<T>, BinaryArchive>
        : public CompositeScribe<Arca::ComputedIndex<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::ComputedIndex<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT&, ArchiveT&) override
        {}
    };
}