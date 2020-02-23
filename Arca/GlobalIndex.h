#pragma once

#include "Index.h"
#include "ReferenceTypeFor.h"
#include "TypeFor.h"
#include "UsableForGlobalIndex.h"

#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Index<T, std::enable_if_t<usable_for_global_index_v<T>>>
    {
    private:
        using StoredT = T*;
    public:
        using ValueT = T;
    public:
        Index() = default;
        explicit Index(Reliquary& owner);
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
        mutable StoredT value = EmptyValue();
    private:
        static StoredT FindValueFrom(Reliquary* reliquary);
        StoredT FindValueFromOwner() const;
        bool IsSetup() const;
        constexpr static StoredT EmptyValue();
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    struct ReferenceTypeFor<T, std::enable_if_t<usable_for_global_index_v<T>>>
    {
        using Type = Index<T, std::enable_if_t<usable_for_global_index_v<T>>>;
    };

    template<class T, std::enable_if_t<usable_for_global_index_v<T>, int> = 0>
    Index<T, std::enable_if_t<usable_for_global_index_v<T>>> ToReference(RelicID, Reliquary& owner)
    {
        return Index<T, std::enable_if_t<usable_for_global_index_v<T>>>(owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_global_index_v<T>>>, BinaryArchive>
        : public CompositeScribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_global_index_v<T>>>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_global_index_v<T>>>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT&, ArchiveT&) override
        {}
    };
}