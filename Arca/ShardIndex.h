#pragma once

#include "Index.h"
#include "ReferenceTypeFor.h"
#include "UsableForShardIndex.h"

#include "TypeFor.h"
#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>
    {
    public:
        using ValueT = T;
    public:
        Index() = default;
        Index(RelicID id, Reliquary& owner);
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
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        [[nodiscard]] ValueT* FindValueFromOwner() const;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::Index(RelicID id, Reliquary& owner) :
        id(id), owner(&owner)
    {}

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::Index(const Index& arg) :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::Index(Index&& arg) noexcept :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator=(const Index& arg) -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator=(Index&& arg) noexcept -> Index&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    bool Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator==(const Index& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator!=(const Index& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator bool() const
    {
        return Get() != nullptr;
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator Handle() const
    {
        return Handle(
            ID(),
            Owner(),
            TypeFor<T, std::enable_if_t<!std::is_const_v<T>>>(),
            HandleObjectTypeFor<T, std::enable_if_t<!std::is_const_v<T>>>());
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator const ValueT* () const
    {
        return Get();
    }

    template<class T>
    Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator Index<const T>() const
    {
        return Index<const T>(id, *owner);
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator*() const -> const ValueT&
    {
        return *Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::operator->() const -> const ValueT*
    {
        return Get();
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::Get() const -> const ValueT*
    {
        return FindValueFromOwner();
    }

    template<class T>
    RelicID Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto Index<T, std::enable_if_t<usable_for_shard_index_v<T> && !std::is_const_v<T>>>::FindValueFromOwner() const -> ValueT*
    {
        if (owner == nullptr)
            return nullptr;

        return owner->template FindStorage<T>(id);
    }

    template<class T>
    class Index<T, std::enable_if_t<usable_for_shard_index_v<T> && std::is_const_v<T>>>
    {
    public:
        using ValueT = T;
    public:
        Index() = default;
        Index(RelicID id, Reliquary& owner);
        Index(const Index& arg);
        Index(Index&& arg) noexcept;

        Index& operator=(const Index& arg);
        Index& operator=(Index&& arg) noexcept;

        bool operator==(const Index& arg) const;
        bool operator!=(const Index& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        operator ValueT* () const
        {
            return Get();
        }

        operator Index<const T>() const;

        [[nodiscard]] ValueT& operator*() const;
        [[nodiscard]] ValueT* operator->() const;

        [[nodiscard]] ValueT* Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary* Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        [[nodiscard]] ValueT* FindValueFromOwner() const;
    private:
        INSCRIPTION_ACCESS;
    };

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

    template<class T>
    struct ReferenceTypeFor<T, std::enable_if_t<usable_for_shard_index_v<T>>>
    {
        using Type = Index<T>;
    };

    template<class T, std::enable_if_t<usable_for_shard_index_v<T>, int> = 0>
    Index<T> ToReference(RelicID id, Reliquary& owner)
    {
        return Index<T>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_shard_index_v<T>>>, BinaryArchive>
        : public CompositeScribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_shard_index_v<T>>>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_shard_index_v<T>>>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            if (archive.IsOutput())
            {
                auto id = object.ID();
                archive(id);
            }
            else
            {
                Arca::RelicID id;
                archive(id);
                object.id = id;

                auto reliquary = archive.template UserContext<Arca::Reliquary>();
                object.owner = reliquary;
                object.value = object.FindValueFromOwner();
            }
        }
    };
}