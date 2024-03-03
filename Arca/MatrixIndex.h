#pragma once

#include "Index.h"
#include "ReferenceTypeFor.h"
#include "RelicID.h"

#include "MatrixImplementation.h"
#include "UsableForMatrixIndex.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class Index<T, std::enable_if_t<usable_for_matrix_index_v<T>>>
    {
    public:
        using ReferenceValueT = typename MatrixImplementation<T>::IndexReference;
        using OptionalValueT = typename MatrixImplementation<T>::IndexOptional;
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

        operator OptionalValueT() const;

        [[nodiscard]] ReferenceValueT operator*() const;
        [[nodiscard]] OptionalValueT operator->() const;

        [[nodiscard]] OptionalValueT Get() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary* Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        [[nodiscard]] OptionalValueT FindValueFromOwner() const;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    struct ReferenceTypeFor<T, std::enable_if_t<usable_for_matrix_index_v<T>>>
    {
        using Type = Index<T, std::enable_if_t<usable_for_matrix_index_v<T>>>;
    };

    template<class T, std::enable_if_t<usable_for_matrix_index_v<T>, int> = 0>
    Index<T, std::enable_if_t<usable_for_matrix_index_v<T>>> ToReference(RelicID id, Reliquary& owner)
    {
        return Index<T, std::enable_if_t<usable_for_matrix_index_v<T>>>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_matrix_index_v<T>>>, BinaryArchive>
        : public CompositeScribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_matrix_index_v<T>>>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::Index<T, std::enable_if_t<Arca::usable_for_matrix_index_v<T>>>, BinaryArchive>;
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