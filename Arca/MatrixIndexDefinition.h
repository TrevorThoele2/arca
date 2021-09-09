#pragma once

#include "MatrixIndex.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    Index<T, std::enable_if_t<is_matrix_v<T>>>::Index(RelicID id, Reliquary& owner, OptionalValueT value) :
        id(id), owner(&owner), value(value)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_matrix_v<T>>>::Index(const Index& arg) :
        id(arg.id), owner(arg.owner), value(arg.value)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_matrix_v<T>>>::Index(Index&& arg) noexcept :
        id(arg.id), owner(arg.owner), value(std::move(arg.value))
    {
        arg.id = nullRelicID;
        arg.owner = nullptr;
        arg.value = MatrixImplementation<T>::DefaultIndexValue();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_matrix_v<T>>>::operator=(const Index& arg) -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_matrix_v<T>>>::operator=(Index&& arg) noexcept -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        value = std::move(arg.value);
        arg.id = nullRelicID;
        arg.owner = nullptr;
        arg.value = MatrixImplementation<T>::DefaultIndexValue();
        return *this;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_matrix_v<T>>>::operator==(const Index& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_matrix_v<T>>>::operator!=(const Index& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Index<T, std::enable_if_t<is_matrix_v<T>>>::operator bool() const
    {
        return static_cast<bool>(Get());
    }

    template<class T>
    Index<T, std::enable_if_t<is_matrix_v<T>>>::operator OptionalValueT() const
    {
        return Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_matrix_v<T>>>::operator*() const -> ReferenceValueT
    {
        return *Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_matrix_v<T>>>::operator->() const -> OptionalValueT
    {
        return Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_matrix_v<T>>>::Get() const -> OptionalValueT
    {
        return value;
    }

    template<class T>
    RelicID Index<T, std::enable_if_t<is_matrix_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* Index<T, std::enable_if_t<is_matrix_v<T>>>::Owner() const
    {
        return owner;
    }
}