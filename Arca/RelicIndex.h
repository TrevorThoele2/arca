#pragma once

#include "Index.h"
#include "UsableForRelicIndex.h"

#include "TypeFor.h"
#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Index<T, std::enable_if_t<usable_for_relic_index_v<T>>>
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
        RelicID id = nullRelicID;
        Reliquary* owner = nullptr;
    private:
        [[nodiscard]] ValueT* FindValueFromOwner() const;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T, std::enable_if_t<usable_for_relic_index_v<T>, int> = 0>
    Index<T, std::enable_if_t<usable_for_relic_index_v<T>>> ToReference(RelicID id, Reliquary& owner)
    {
        return Index<T, std::enable_if_t<usable_for_relic_index_v<T>>>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_relic_index_v<T>>>>
    {
    public:
        using ObjectT = Arca::Index<T, std::enable_if_t<Arca::usable_for_relic_index_v<T>>>;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
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
            }
        }
    };

    template<class T, class Archive>
    struct ScribeTraits<Arca::Index<T, std::enable_if_t<Arca::usable_for_relic_index_v<T>>>, Archive> final
    {
        using Category = CompositeScribeCategory<Arca::Index<T, std::enable_if_t<Arca::usable_for_relic_index_v<T>>>>;
    };
}