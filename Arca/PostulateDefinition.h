#pragma once

#include "Postulate.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::Postulate(Reliquary& owner) :
        owner(&owner), value(FindValueFrom(&owner))
    {}

    template<class T>
    Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::Postulate(const Postulate& arg) :
        owner(arg.owner), value(FindValueFrom(arg.owner))
    {}

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
    auto Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::FindValueFrom(Reliquary* reliquary) -> StoredT
    {
        if (reliquary == nullptr)
            return EmptyValue();

        return reliquary->template FindPostulateValue<T>();
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<!std::is_pointer_v<T>>>::FindValueFromOwner() const -> StoredT
    {
        return FindValueFrom(owner);
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
    Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::Postulate(Reliquary& owner) :
        owner(&owner), value(FindValueFrom(&owner))
    {}

    template<class T>
    Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::Postulate(const Postulate& arg) :
        owner(arg.owner), value(FindValueFrom(arg.owner))
    {}

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
    auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::FindValueFrom(Reliquary* reliquary) -> StoredT
    {
        if (reliquary == nullptr)
            return EmptyValue();

        return reliquary->template FindPostulateValue<T>();
    }

    template<class T>
    auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::FindValueFromOwner() const -> StoredT
    {
        return FindValueFrom(owner);
    }

    template<class T>
    bool Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::IsSetup() const
    {
        return value != nullptr;
    }

    template<class T>
    constexpr auto Postulate<T, std::enable_if_t<std::is_pointer_v<T>>>::EmptyValue() -> StoredT
    {
        return nullptr;
    }
}