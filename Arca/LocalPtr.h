#pragma once

#include "IsLocal.h"
#include "IsEither.h"

#include "TypeFor.h"
#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_local_ptr_v =
        (is_relic_v<T> && is_local_v<T>) || is_either_v<T> || is_shard_v<T>;

    template<class T, class = void>
    struct LocalPtrValueT
    {
        using Type = T;
    };

    template<class T>
    struct LocalPtrValueT<T, std::enable_if_t<is_either_v<T>>>
    {
        using Type = typename T::ShardT;
    };

    template<class T>
    class LocalPtr
    {
    public:
        using ValueT = typename LocalPtrValueT<T>::Type;
    public:
        LocalPtr() = default;

        LocalPtr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
        {
            value = FindValueFromOwner();
        }

        LocalPtr(const LocalPtr& arg) : id(arg.id), owner(arg.owner)
        {
            value = FindValueFromOwner();
        }

        LocalPtr(LocalPtr&& arg) noexcept : id(arg.id), owner(arg.owner), value(arg.value)
        {}

        LocalPtr& operator=(const LocalPtr& arg)
        {
            id = arg.id;
            owner = arg.owner;
            value = FindValueFromOwner();
            return *this;
        }

        LocalPtr& operator=(LocalPtr&& arg) noexcept
        {
            id = arg.id;
            owner = arg.owner;
            value = arg.value;
            return *this;
        }

        bool operator==(const LocalPtr& arg) const
        {
            return id == arg.id && owner == arg.owner;
        }

        bool operator!=(const LocalPtr& arg) const
        {
            return !(*this == arg);
        }

        explicit operator bool() const
        {
            return value != nullptr;
        }

        operator Handle() const
        {
            return Handle(ID(), Owner(), TypeFor<T>(), HandleObjectTypeFor<T>());
        }

        operator ValueT* () const
        {
            return Get();
        }

        operator LocalPtr<const T>() const
        {
            return LocalPtr<const T>(id, *owner);
        }

        [[nodiscard]] ValueT& operator*() const
        {
            return *Get();
        }

        [[nodiscard]] ValueT* operator->() const
        {
            return Get();
        }

        [[nodiscard]] ValueT* Get() const
        {
            if (value == nullptr)
                value = FindValueFromOwner();

            return value;
        }

        [[nodiscard]] RelicID ID() const
        {
            return id;
        }

        [[nodiscard]] Reliquary* Owner() const
        {
            return owner;
        }
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
        mutable ValueT* value = nullptr;
    private:
        ValueT* FindValueFromOwner() const
        {
            if (owner == nullptr)
                return nullptr;

            return owner->template FindStorage<T>(id);
        }
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::LocalPtr<T>, BinaryArchive>
        : public CompositeScribe<Arca::LocalPtr<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::LocalPtr<T>, BinaryArchive>;
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