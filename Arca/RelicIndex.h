#pragma once

#include "Index.h"

#include "IsRelic.h"
#include "IsLocal.h"
#include "Handle.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Index<T, std::enable_if_t<is_relic_v<T> && is_local_v<T>>>
    {
    public:
        using ValueT = T;
    public:
        Index() = default;
        Index(RelicID id, Reliquary& owner, std::weak_ptr<ValueT> value);
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
        RelicID id = nullRelicID;
        Reliquary* owner = nullptr;
        std::weak_ptr<ValueT> value;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::Index(RelicID id, Reliquary& owner, std::weak_ptr<ValueT> value) :
        id(id), owner(&owner), value(value)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::Index(const Index& arg) :
        id(arg.id), owner(arg.owner), value(arg.value)
    {}

    template<class T>
    Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::Index(Index&& arg) noexcept :
        id(arg.id), owner(arg.owner), value(std::move(arg.value))
    {
        arg.id = nullRelicID;
        arg.owner = nullptr;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator=(const Index& arg) -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        value = arg.value;
        return *this;
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator=(Index&& arg) noexcept -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        value = std::move(arg.value);
        arg.id = 0;
        arg.owner = nullptr;
        return *this;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator==(const Index& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator!=(const Index& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator bool() const
    {
        return Get() != nullptr;
    }

    template<class T>
    Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator Handle() const
    {
        return Handle{ ID(), TypeFor<T>() };
    }

    template<class T>
    Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator const ValueT* () const
    {
        return Get();
    }

    template<class T>
    Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator Index<const T>() const
    {
        return Index<const T>(id, *owner, value);
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator*() const -> const ValueT&
    {
        return *Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::operator->() const -> const ValueT*
    {
        return Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::Get() const -> const ValueT*
    {
        return value.lock().get();
    }

    template<class T>
    RelicID Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* Index<T, std::enable_if_t<is_relic_v<T>&& is_local_v<T>>>::Owner() const
    {
        return owner;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Index<T, std::enable_if_t<Arca::is_relic_v<T> && Arca::is_local_v<T>>>>
    {
    public:
        using ObjectT = Arca::Index<T, std::enable_if_t<Arca::is_relic_v<T> && Arca::is_local_v<T>>>;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive);
    };

    template<class T, class Archive>
    struct ScribeTraits<Arca::Index<T, std::enable_if_t<Arca::is_relic_v<T> && Arca::is_local_v<T>>>, Archive> final
    {
        using Category = CompositeScribeCategory<Arca::Index<T, std::enable_if_t<Arca::is_relic_v<T> && Arca::is_local_v<T>>>>;
    };
}