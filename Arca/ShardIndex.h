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

                auto context = archive.template UserContext<Arca::InscriptionUserContext>();
                object.owner = context->reliquary;
            }
        }
    };
}