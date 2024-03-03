#pragma once

#include "ShardIndex.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::Index(RelicID id, Reliquary& owner) :
        id(id), owner(&owner)
    {}

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::Index(const Index& arg) :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::Index(Index&& arg) noexcept :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::operator=(const Index& arg) -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::operator=(Index&& arg) noexcept -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    bool Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::operator==(const Index& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::operator!=(const Index& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::operator bool() const
    {
        return Get() != nullptr;
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::operator Handle() const
    {
        return Handle(
            ID(),
            Owner(),
            TypeFor<T, std::enable_if_t<std::is_const_v<T>>>(),
            HandleObjectTypeFor<T, std::enable_if_t<std::is_const_v<T>>>());
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::operator Index<const T>() const
    {
        return Index<const T>(id, *owner);
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::operator*() const -> ValueT&
    {
        return *Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::operator->() const -> ValueT*
    {
        return Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::Get() const -> ValueT*
    {
        return FindValueFromOwner();
    }

    template<class T>
    RelicID Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>::FindValueFromOwner() const -> ValueT*
    {
        if (owner == nullptr)
            return nullptr;

        return owner->template FindStorage<T>(id);
    }
}