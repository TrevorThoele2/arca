#pragma once

#include "Index.h"
#include "ReferenceTypeFor.h"
#include "TypeFor.h"
#include "UsableForGlobalIndex.h"

#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Index<T, std::enable_if_t<usable_for_global_index_v<T>>>
    {
    private:
        using StoredT = T*;
    public:
        using ValueT = T;
    public:
        Index() = default;
        explicit Index(Reliquary& owner);
        Index(const Index& arg);
        Index(Index&& arg) noexcept;

        Index& operator=(const Index& arg);
        Index& operator=(Index&& arg) noexcept;

        bool operator==(const Index& arg) const;
        bool operator!=(const Index& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        operator const ValueT* () const;

        operator Index<const T>() const;

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
    Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::Index(Reliquary& owner) : owner(&owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::Index(const Index& arg) : owner(arg.owner)
    {
        value = FindValueFromOwner();
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::Index(Index&& arg) noexcept :
        owner(arg.owner), value(arg.value)
    {}

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator=(const Index& arg) -> Index&
    {
        owner = arg.owner;
        value = FindValueFromOwner();
        return *this;
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator=(Index&& arg) noexcept -> Index&
    {
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

    template<class T>
    bool Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator==(const Index& arg) const
    {
        return owner == arg.owner;
    }

    template<class T>
    bool Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator!=(const Index& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator bool() const
    {
        return IsSetup();
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator Handle() const
    {
        if (Owner() == nullptr)
            return {};

        return Handle(ID(), Owner(), TypeFor<T>(), HandleObjectType::Relic);
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator const ValueT* () const
    {
        return Get();
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator Index<const T>() const
    {
        return Index<const T>(*owner);
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator*() const -> const ValueT&
    {
        return *Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::operator->() const -> const ValueT*
    {
        return Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::Get() const -> const ValueT*
    {
        if (!IsSetup())
            value = FindValueFromOwner();

        return value;
    }

    template<class T>
    RelicID Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::ID() const
    {
        if (!Owner())
            return 0;

        return Owner()->template IDFor<T>();
    }

    template<class T>
    Reliquary* Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::FindValueFromOwner() const -> StoredT
    {
        if (owner == nullptr)
            return EmptyValue();

        return owner->template FindGlobalStorage<T>();
    }

    template<class T>
    bool Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::IsSetup() const
    {
        return value != nullptr;
    }

    template<class T>
    constexpr auto Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::EmptyValue() -> StoredT
    {
        return nullptr;
    }

    template<class T>
    struct ReferenceTypeFor<T, std::enable_if_t<usable_for_global_index_v<T>>>
    {
        using Type = Index<T, std::enable_if_t<usable_for_global_index_v<T>>>;
    };

    template<class T, std::enable_if_t<usable_for_global_index_v<T>, int> = 0>
    Index<T, std::enable_if_t<usable_for_global_index_v<T>>> ToReference(RelicID, Reliquary& owner)
    {
        return Index<T, std::enable_if_t<usable_for_global_index_v<T>>>(owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_global_index_v<T>>>, BinaryArchive>
        : public CompositeScribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_global_index_v<T>>>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_global_index_v<T>>>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT&, ArchiveT&) override
        {}
    };
}