#pragma once

#include "Index.h"
#include "RelicID.h"

#include "MatrixImplementation.h"
#include "IsMatrix.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Index<T, std::enable_if_t<is_matrix_v<T>>>
    {
    public:
        using ReferenceValueT = typename MatrixImplementation<T>::IndexReference;
        using OptionalValueT = typename MatrixImplementation<T>::IndexOptional;
    public:
        Index() = default;
        Index(RelicID id, Reliquary& owner, OptionalValueT value);
        Index(const Index& arg);
        Index(Index&& arg) noexcept;

        Index& operator=(const Index& arg);
        Index& operator=(Index&& arg) noexcept;

        bool operator==(const Index& arg) const;
        bool operator!=(const Index& arg) const;

        explicit operator bool() const;

        operator OptionalValueT() const;

        [[nodiscard]] ReferenceValueT operator*() const;
        [[nodiscard]] OptionalValueT operator->() const;

        [[nodiscard]] OptionalValueT Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary* Owner() const;
    private:
        RelicID id = nullRelicID;
        Reliquary* owner = nullptr;
        OptionalValueT value;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T, std::enable_if_t<is_matrix_v<T>, int> = 0>
    Index<T, std::enable_if_t<is_matrix_v<T>>> ToIndex(RelicID id, Reliquary* owner)
    {
        return Index<T, std::enable_if_t<is_matrix_v<T>>>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Index<T, std::enable_if_t<Arca::is_matrix_v<T>>>>
    {
    public:
        using ObjectT = Arca::Index<T, std::enable_if_t<Arca::is_matrix_v<T>>>;
    protected:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {
            if (format.IsOutput())
            {
                auto id = object.ID();
                format("id", id);
            }
            else
            {
                Arca::RelicID id;
                format("id", id);
                object.id = id;

                object.owner = format.template UserContext<ReliquaryUserContext>()->reliquary;
            }
        }
    };

    template<class T, class Format>
    struct ScribeTraits<Arca::Index<T, std::enable_if_t<Arca::is_matrix_v<T>>>, Format> final
    {
        using Category = CompositeScribeCategory<Arca::Index<T, std::enable_if_t<Arca::is_matrix_v<T>>>>;
    };
}