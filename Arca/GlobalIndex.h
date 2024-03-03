#pragma once

#include "Index.h"
#include "TypeFor.h"
#include "IsGlobal.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Index<T, std::enable_if_t<is_global_v<T>>>
    {
    public:
        using ValueT = T;
    public:
        Index() = default;
        explicit Index(Reliquary& owner, std::weak_ptr<ValueT> value);
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
        Reliquary* owner = nullptr;
        std::weak_ptr<ValueT> value;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T, std::enable_if_t<is_global_v<T>, int> = 0>
    Index<T, std::enable_if_t<is_global_v<T>>> ToIndex(RelicID, Reliquary* owner)
    {
        return Index<T, std::enable_if_t<is_global_v<T>>>(owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Index<T, std::enable_if_t<Arca::is_global_v<T>>>>
    {
    public:
        using ObjectT = Arca::Index<T, std::enable_if_t<Arca::is_global_v<T>>>;
    public:
        template<class Format>
        void Scriven(ObjectT&, Format&)
        {}
    };

    template<class T, class Format>
    struct ScribeTraits<Arca::Index<T, std::enable_if_t<Arca::is_global_v<T>>>, Format> final
    {
        using Category = CompositeScribeCategory<Arca::Index<T, std::enable_if_t<Arca::is_global_v<T>>>>;
    };
}