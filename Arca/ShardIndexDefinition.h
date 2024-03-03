#pragma once

#include "ShardIndex.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::Index(RelicID id, Reliquary& owner, std::weak_ptr<ValueT> value) :
        id(id), owner(&owner), value(value)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::Index(const Index& arg) :
        id(arg.id), owner(arg.owner), value(arg.value), justLoaded(arg.justLoaded)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::Index(Index&& arg) noexcept :
        id(arg.id), owner(arg.owner), value(std::move(arg.value)), justLoaded(arg.justLoaded)
    {
        arg.id = nullRelicID;
        arg.owner = nullptr;
        arg.justLoaded = false;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator=(const Index& arg) -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        value = arg.value;
        justLoaded = arg.justLoaded;
        return *this;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator=(Index&& arg) noexcept -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        value = std::move(arg.value);
        justLoaded = arg.justLoaded;
        arg.id = nullRelicID;
        arg.owner = nullptr;
        arg.justLoaded = false;
        return *this;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator==(const Index& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator!=(const Index& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator bool() const
    {
        return Get() != nullptr;
    }

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator Handle() const
    {
        return Handle{ ID(), TypeFor<T>() };
    }

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator const ValueT* () const
    {
        return Get();
    }

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator Index<const T>() const
    {
        auto index = Index<const T>(id, *owner, value);
        index.justLoaded = justLoaded;
        return index;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator*() const -> const ValueT&
    {
        return *Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::operator->() const -> const ValueT*
    {
        return Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::Get() const -> const ValueT*
    {
        if (justLoaded)
        {
            value = owner->Find<T>(id).value;
            justLoaded = false;
        }

        return value.lock().get();
    }

    template<class T>
    RelicID Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::Index(RelicID id, Reliquary& owner, std::weak_ptr<ValueT> value) :
        id(id), owner(&owner), value(value)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::Index(const Index& arg) :
        id(arg.id), owner(arg.owner), value(arg.value), justLoaded(arg.justLoaded)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::Index(Index&& arg) noexcept :
        id(arg.id), owner(arg.owner), value(std::move(arg.value)), justLoaded(arg.justLoaded)
    {
        arg.id = nullRelicID;
        arg.owner = nullptr;
        arg.justLoaded = false;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::operator=(const Index& arg) -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        value = arg.value;
        justLoaded = arg.justLoaded;
        return *this;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::operator=(Index&& arg) noexcept -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        value = std::move(arg.value);
        justLoaded = arg.justLoaded;
        arg.id = nullRelicID;
        arg.owner = nullptr;
        arg.justLoaded = false;
        return *this;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::operator==(const Index& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::operator!=(const Index& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::operator bool() const
    {
        return Get() != nullptr;
    }

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::operator Handle() const
    {
        return Handle{ id, TypeFor<T, std::enable_if_t<std::is_const_v<T>>>() };
    }

    template<class T>
    Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::operator Index<const T>() const
    {
        auto index = Index<const T>(id, *owner, value);
        index.justLoaded = justLoaded;
        return index;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::operator*() const -> ValueT&
    {
        return *Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::operator->() const -> ValueT*
    {
        return Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::Get() const -> ValueT*
    {
        if (justLoaded)
        {
            value = owner->Find<T>(id).value;
            justLoaded = false;
        }

        return value.lock().get();
    }

    template<class T>
    RelicID Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* Index<T, std::enable_if_t<is_shard_v<T>&& std::is_const_v<T>>>::Owner() const
    {
        return owner;
    }
}