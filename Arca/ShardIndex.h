#pragma once

#include "Index.h"

#include "IsShard.h"
#include "Handle.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Index<T, std::enable_if_t<is_shard_v<T> && !std::is_const_v<T>>>
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
        mutable std::weak_ptr<T> value;
        mutable bool justLoaded = false;
    private:
        INSCRIPTION_ACCESS;
    };
    
    template<class T>
    class Index<T, std::enable_if_t<is_shard_v<T> && std::is_const_v<T>>>
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
        RelicID id = nullRelicID;
        Reliquary* owner = nullptr;
        mutable std::weak_ptr<T> value;
        mutable bool justLoaded = false;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Index<T, std::enable_if_t<Arca::is_shard_v<T>>>>
    {
    public:
        using ObjectT = Arca::Index<T, std::enable_if_t<Arca::is_shard_v<T>>>;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive);
    };

    template<class T>
    template<class Archive>
    void Scribe<Arca::Index<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Scriven(ObjectT& object, Archive& archive)
    {
        if (archive.IsOutput())
        {
            auto id = object.ID();
            archive("id", id);
        }
        else
        {
            Arca::RelicID id;
            archive("id", id);
            object.id = id;

            object.owner = archive.template UserContext<ReliquaryUserContext>()->reliquary;
            object.justLoaded = true;
        }
    }

    template<class T, class Archive>
    struct ScribeTraits<Arca::Index<T, std::enable_if_t<Arca::is_shard_v<T>>>, Archive> final
    {
        using Category = CompositeScribeCategory<Arca::Index<T, std::enable_if_t<Arca::is_shard_v<T>>>>;
    };
}