#pragma once

#include "GlobalIndex.h"
#include "Reliquary.h"

namespace Arca
{
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
    {
        arg.value = nullptr;
        arg.owner = nullptr;
    }

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
        arg.owner = nullptr;
        arg.value = nullptr;
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
            return nullRelicID;

        return Owner()->template IDFor<T>();
    }

    template<class T>
    Reliquary* Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::FindValueFrom(Reliquary* reliquary) -> StoredT
    {
        if (reliquary == nullptr)
            return EmptyValue();

        return reliquary->template FindGlobalStorage<T>();
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_global_index_v<T>>>::FindValueFromOwner() const -> StoredT
    {
        return FindValueFrom(owner);
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
}