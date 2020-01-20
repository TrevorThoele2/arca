#pragma once

#include "IndexTypeFor.h"
#include "UsableForRelicIndex.h"

#include "TypeFor.h"
#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class RelicIndex
    {
    public:
        using ValueT = T;
    public:
        RelicIndex() = default;
        RelicIndex(RelicID id, Reliquary& owner);
        RelicIndex(const RelicIndex& arg);
        RelicIndex(RelicIndex&& arg) noexcept;

        RelicIndex& operator=(const RelicIndex& arg);
        RelicIndex& operator=(RelicIndex&& arg) noexcept;

        bool operator==(const RelicIndex& arg) const;
        bool operator!=(const RelicIndex& arg) const;

        explicit operator bool() const;

        operator Handle() const;

        operator ValueT* () const
        {
            return Get();
        }

        operator RelicIndex<const T>() const;

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
    RelicIndex<T>::RelicIndex(RelicID id, Reliquary& owner) :
        id(id), owner(&owner)
    {}

    template<class T>
    RelicIndex<T>::RelicIndex(const RelicIndex& arg) :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    RelicIndex<T>::RelicIndex(RelicIndex&& arg) noexcept :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    auto RelicIndex<T>::operator=(const RelicIndex& arg) -> RelicIndex&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    auto RelicIndex<T>::operator=(RelicIndex&& arg) noexcept -> RelicIndex&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    bool RelicIndex<T>::operator==(const RelicIndex& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool RelicIndex<T>::operator!=(const RelicIndex& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    RelicIndex<T>::operator bool() const
    {
        return Get() != nullptr;
    }

    template<class T>
    RelicIndex<T>::operator Handle() const
    {
        return Handle(ID(), Owner(), TypeFor<T>(), HandleObjectTypeFor<T>());
    }

    template<class T>
    RelicIndex<T>::operator RelicIndex<const T>() const
    {
        return RelicIndex<const T>(id, *owner);
    }

    template<class T>
    auto RelicIndex<T>::operator*() const -> ValueT&
    {
        return *Get();
    }

    template<class T>
    auto RelicIndex<T>::operator->() const -> ValueT*
    {
        return Get();
    }

    template<class T>
    auto RelicIndex<T>::Get() const -> ValueT*
    {
        return FindValueFromOwner();
    }

    template<class T>
    RelicID RelicIndex<T>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* RelicIndex<T>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto RelicIndex<T>::FindValueFromOwner() const -> ValueT*
    {
        if (owner == nullptr)
            return nullptr;

        return owner->template FindStorage<T>(id);
    }

    template<class T>
    struct IndexTypeFor<T, std::enable_if_t<usable_for_relic_index_v<T>>>
    {
        using Type = RelicIndex<T>;
    };

    template<class T, std::enable_if_t<usable_for_relic_index_v<T>, int> = 0>
    RelicIndex<T> ToIndex(RelicID id, Reliquary& owner)
    {
        return RelicIndex<T>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::RelicIndex<T>, BinaryArchive>
        : public CompositeScribe<Arca::RelicIndex<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::RelicIndex<T>, BinaryArchive>;
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