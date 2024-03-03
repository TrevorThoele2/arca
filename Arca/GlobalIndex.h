#pragma once

#include "IndexTypeFor.h"
#include "TypeFor.h"
#include "UsableForGlobalIndex.h"

#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class GlobalIndex
    {
    private:
        using StoredT = T*;
    public:
        using ValueT = T;
    public:
        GlobalIndex() = default;
        explicit GlobalIndex(Reliquary& owner);
        GlobalIndex(const GlobalIndex& arg);
        GlobalIndex(GlobalIndex&& arg) noexcept;

        GlobalIndex& operator=(const GlobalIndex& arg);
        GlobalIndex& operator=(GlobalIndex&& arg) noexcept;

        bool operator==(const GlobalIndex& arg) const;
        bool operator!=(const GlobalIndex& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        operator const ValueT* () const;

        operator GlobalIndex<const T>() const;

        [[nodiscard]] const ValueT& operator*() const;
        [[nodiscard]] const ValueT* operator->() const;

        [[nodiscard]] const ValueT* Get() const;

        [[nodiscard]] RelicID ID() const;
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
    GlobalIndex<T>::GlobalIndex(Reliquary& owner) : owner(&owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    GlobalIndex<T>::GlobalIndex(const GlobalIndex& arg) : owner(arg.owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    GlobalIndex<T>::GlobalIndex(GlobalIndex&& arg) noexcept :
        owner(arg.owner), value(arg.value)
    {}

    template<class T>
    auto GlobalIndex<T>::operator=(const GlobalIndex& arg) -> GlobalIndex&
    {
        owner = arg.owner;
        value = FindValueFromOwner();
        return *this;
    }

    template<class T>
    auto GlobalIndex<T>::operator=(GlobalIndex&& arg) noexcept -> GlobalIndex&
    {
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

    template<class T>
    bool GlobalIndex<T>::operator==(const GlobalIndex& arg) const
    {
        return owner == arg.owner;
    }

    template<class T>
    bool GlobalIndex<T>::operator!=(const GlobalIndex& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    GlobalIndex<T>::operator bool() const
    {
        return IsSetup();
    }

    template<class T>
    GlobalIndex<T>::operator Handle() const
    {
        if (Owner() == nullptr)
            return {};

        return Handle(ID(), Owner(), TypeFor<T>(), HandleObjectType::Relic);
    }

    template<class T>
    GlobalIndex<T>::operator const ValueT* () const
    {
        return Get();
    }

    template<class T>
    GlobalIndex<T>::operator GlobalIndex<const T>() const
    {
        return GlobalIndex<const T>(*owner);
    }

    template<class T>
    auto GlobalIndex<T>::operator*() const -> const ValueT&
    {
        return *Get();
    }

    template<class T>
    auto GlobalIndex<T>::operator->() const -> const ValueT*
    {
        return Get();
    }

    template<class T>
    auto GlobalIndex<T>::Get() const -> const ValueT*
    {
        if (!IsSetup())
            value = FindValueFromOwner();

        return value;
    }

    template<class T>
    RelicID GlobalIndex<T>::ID() const
    {
        if (!Owner())
            return 0;

        return Owner()->template IDFor<T>();
    }

    template<class T>
    Reliquary* GlobalIndex<T>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto GlobalIndex<T>::FindValueFromOwner() const -> StoredT
    {
        if (owner == nullptr)
            return EmptyValue();

        return owner->template FindGlobalStorage<T>();
    }

    template<class T>
    bool GlobalIndex<T>::IsSetup() const
    {
        return value != nullptr;
    }

    template<class T>
    constexpr auto GlobalIndex<T>::EmptyValue() -> StoredT
    {
        return nullptr;
    }

    template<class T>
    struct IndexTypeFor<T, std::enable_if_t<usable_for_global_index_v<T>>>
    {
        using Type = GlobalIndex<T>;
    };

    template<class T, std::enable_if_t<usable_for_global_index_v<T>, int> = 0>
    GlobalIndex<T> ToIndex(RelicID, Reliquary& owner)
    {
        return GlobalIndex<T>(owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::GlobalIndex<T>, BinaryArchive>
        : public CompositeScribe<Arca::GlobalIndex<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::GlobalIndex<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT&, ArchiveT&) override
        {}
    };
}