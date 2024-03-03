#pragma once

#include "IsEither.h"
#include "UsableForLocalPtr.h"

#include "TypeFor.h"
#include "PtrTypeFor.h"
#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
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
        {}

        LocalPtr(const LocalPtr& arg) : id(arg.id), owner(arg.owner)
        {}

        LocalPtr(LocalPtr&& arg) noexcept : id(arg.id), owner(arg.owner)
        {}

        LocalPtr& operator=(const LocalPtr& arg)
        {
            id = arg.id;
            owner = arg.owner;
            return *this;
        }

        LocalPtr& operator=(LocalPtr&& arg) noexcept
        {
            id = arg.id;
            owner = arg.owner;
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
            return Get() != nullptr;
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
            return FindValueFromOwner();
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
    private:
        [[nodiscard]] ValueT* FindValueFromOwner() const
        {
            if (owner == nullptr)
                return nullptr;

            return owner->template FindStorage<T>(id);
        }
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    struct PtrTypeFor<T, std::enable_if_t<usable_for_local_ptr_v<T>>>
    {
        using Type = LocalPtr<T>;
    };

    template<class T, std::enable_if_t<usable_for_local_ptr_v<T>, int> = 0>
    LocalPtr<T> ToPtr(RelicID id, Reliquary& owner)
    {
        return LocalPtr<T>(id, owner);
    }
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