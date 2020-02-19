#pragma once

#include "IndexTypeFor.h"
#include "UsableForShardIndex.h"

#include "TypeFor.h"
#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T, class Enable = void>
    class ShardIndex;

    template<class T>
    class ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>
    {
    public:
        using ValueT = T;
    public:
        ShardIndex() = default;
        ShardIndex(RelicID id, Reliquary& owner);
        ShardIndex(const ShardIndex& arg);
        ShardIndex(ShardIndex&& arg) noexcept;

        ShardIndex& operator=(const ShardIndex& arg);
        ShardIndex& operator=(ShardIndex&& arg) noexcept;

        bool operator==(const ShardIndex& arg) const;
        bool operator!=(const ShardIndex& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        operator const ValueT* () const;

        operator ShardIndex<const T>() const;

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
    ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::ShardIndex(RelicID id, Reliquary& owner) :
        id(id), owner(&owner)
    {}

    template<class T>
    ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::ShardIndex(const ShardIndex& arg) :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::ShardIndex(ShardIndex&& arg) noexcept :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    auto ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator=(const ShardIndex& arg) -> ShardIndex&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator=(ShardIndex&& arg) noexcept -> ShardIndex&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    bool ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator==(const ShardIndex& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator!=(const ShardIndex& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator bool() const
    {
        return Get() != nullptr;
    }

    template<class T>
    ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator Handle() const
    {
        return Handle(
            ID(),
            Owner(),
            TypeFor<T, std::enable_if_t<!std::is_const_v<T>>>(),
            HandleObjectTypeFor<T, std::enable_if_t<!std::is_const_v<T>>>());
    }

    template<class T>
    ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator const ValueT* () const
    {
        return Get();
    }

    template<class T>
    ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator ShardIndex<const T>() const
    {
        return ShardIndex<const T>(id, *owner);
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator*() const -> const ValueT&
    {
        return *Get();
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::operator->() const -> const ValueT*
    {
        return Get();
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::Get() const -> const ValueT*
    {
        return FindValueFromOwner();
    }

    template<class T>
    RelicID ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<!std::is_const_v<T>>>::FindValueFromOwner() const -> ValueT*
    {
        if (owner == nullptr)
            return nullptr;

        return owner->template FindStorage<T>(id);
    }

    template<class T>
    class ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>
    {
    public:
        using ValueT = T;
    public:
        ShardIndex() = default;
        ShardIndex(RelicID id, Reliquary& owner);
        ShardIndex(const ShardIndex& arg);
        ShardIndex(ShardIndex&& arg) noexcept;

        ShardIndex& operator=(const ShardIndex& arg);
        ShardIndex& operator=(ShardIndex&& arg) noexcept;

        bool operator==(const ShardIndex& arg) const;
        bool operator!=(const ShardIndex& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        operator ValueT* () const
        {
            return Get();
        }

        operator ShardIndex<const T>() const;

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
    ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::ShardIndex(RelicID id, Reliquary& owner) :
        id(id), owner(&owner)
    {}

    template<class T>
    ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::ShardIndex(const ShardIndex& arg) :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::ShardIndex(ShardIndex&& arg) noexcept :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    auto ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::operator=(const ShardIndex& arg) -> ShardIndex&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::operator=(ShardIndex&& arg) noexcept -> ShardIndex&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    bool ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::operator==(const ShardIndex& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::operator!=(const ShardIndex& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::operator bool() const
    {
        return Get() != nullptr;
    }

    template<class T>
    ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::operator Handle() const
    {
        return Handle(
            ID(),
            Owner(),
            TypeFor<T, std::enable_if_t<std::is_const_v<T>>>(),
            HandleObjectTypeFor<T, std::enable_if_t<std::is_const_v<T>>>());
    }

    template<class T>
    ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::operator ShardIndex<const T>() const
    {
        return ShardIndex<const T>(id, *owner);
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::operator*() const -> ValueT&
    {
        return *Get();
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::operator->() const -> ValueT*
    {
        return Get();
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::Get() const -> ValueT*
    {
        return FindValueFromOwner();
    }

    template<class T>
    RelicID ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto ShardIndex<T, std::enable_if_t<std::is_const_v<T>>>::FindValueFromOwner() const -> ValueT*
    {
        if (owner == nullptr)
            return nullptr;

        return owner->template FindStorage<T>(id);
    }

    template<class T>
    struct IndexTypeFor<T, std::enable_if_t<usable_for_shard_index_v<T>>>
    {
        using Type = ShardIndex<T>;
    };

    template<class T, std::enable_if_t<usable_for_shard_index_v<T>, int> = 0>
    ShardIndex<T> ToReference(RelicID id, Reliquary& owner)
    {
        return ShardIndex<T>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::ShardIndex<T>, BinaryArchive>
        : public CompositeScribe<Arca::ShardIndex<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::ShardIndex<T>, BinaryArchive>;
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