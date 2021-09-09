#pragma once

#include "GlobalIndex.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    Index<T, std::enable_if_t<is_global_v<T>>>::Index(Reliquary& owner, std::weak_ptr<ValueT> value) : owner(&owner), value(value)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_global_v<T>>>::Index(const Index& arg) : owner(arg.owner), value(arg.value)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_global_v<T>>>::Index(Index&& arg) noexcept :
        owner(arg.owner), value(std::move(arg.value))
    {
        arg.owner = nullptr;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_global_v<T>>>::operator=(const Index& arg) -> Index&
    {
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_global_v<T>>>::operator=(Index&& arg) noexcept -> Index&
    {
        owner = arg.owner;
        value = std::move(arg.value);
        arg.owner = nullptr;
        return *this;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_global_v<T>>>::operator==(const Index& arg) const
    {
        return owner == arg.owner;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_global_v<T>>>::operator!=(const Index& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Index<T, std::enable_if_t<is_global_v<T>>>::operator bool() const
    {
        return static_cast<bool>(value.lock());
    }

    template<class T>
    Index<T, std::enable_if_t<is_global_v<T>>>::operator Handle() const
    {
        return Owner() == nullptr ? Handle{} : Handle{ ID(), TypeFor<T>() };
    }

    template<class T>
    Index<T, std::enable_if_t<is_global_v<T>>>::operator const ValueT* () const
    {
        return Get();
    }

    template<class T>
    Index<T, std::enable_if_t<is_global_v<T>>>::operator Index<const T>() const
    {
        return Index<const T>(*owner, value);
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_global_v<T>>>::operator*() const -> const ValueT&
    {
        return *Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_global_v<T>>>::operator->() const -> const ValueT*
    {
        return Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_global_v<T>>>::Get() const -> const ValueT*
    {
        return value.lock().get();
    }

    template<class T>
    RelicID Index<T, std::enable_if_t<is_global_v<T>>>::ID() const
    {
        return !Owner() ? nullRelicID : Owner()->IDFor<T>();
    }

    template<class T>
    Reliquary* Index<T, std::enable_if_t<is_global_v<T>>>::Owner() const
    {
        return owner;
    }
}