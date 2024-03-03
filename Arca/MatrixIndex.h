#pragma once

#include "IndexTypeFor.h"
#include "RelicID.h"

#include "MatrixImplementation.h"
#include "UsableForMatrixIndex.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class MatrixIndex
    {
    public:
        using ReferenceValueT = typename MatrixImplementation<T>::IndexReference;
        using OptionalValueT = typename MatrixImplementation<T>::IndexOptional;
    public:
        MatrixIndex() = default;
        MatrixIndex(RelicID id, Reliquary& owner);
        MatrixIndex(const MatrixIndex& arg);
        MatrixIndex(MatrixIndex&& arg) noexcept;

        MatrixIndex& operator=(const MatrixIndex& arg);
        MatrixIndex& operator=(MatrixIndex&& arg) noexcept;

        bool operator==(const MatrixIndex& arg) const;
        bool operator!=(const MatrixIndex& arg) const;

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
    MatrixIndex<T>::MatrixIndex(RelicID id, Reliquary& owner) :
        id(id), owner(&owner)
    {}

    template<class T>
    MatrixIndex<T>::MatrixIndex(const MatrixIndex& arg) :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    MatrixIndex<T>::MatrixIndex(MatrixIndex&& arg) noexcept :
        id(arg.id), owner(arg.owner)
    {}

    template<class T>
    auto MatrixIndex<T>::operator=(const MatrixIndex& arg) -> MatrixIndex&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    auto MatrixIndex<T>::operator=(MatrixIndex&& arg) noexcept -> MatrixIndex&
    {
        id = arg.id;
        owner = arg.owner;
        return *this;
    }

    template<class T>
    bool MatrixIndex<T>::operator==(const MatrixIndex& arg) const
    {
        return id == arg.id && owner == arg.owner;
    }

    template<class T>
    bool MatrixIndex<T>::operator!=(const MatrixIndex& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    MatrixIndex<T>::operator bool() const
    {
        return static_cast<bool>(Get());
    }

    template<class T>
    MatrixIndex<T>::operator OptionalValueT() const
    {
        return Get();
    }

    template<class T>
    auto MatrixIndex<T>::operator*() const -> ReferenceValueT
    {
        return *Get();
    }

    template<class T>
    auto MatrixIndex<T>::operator->() const -> OptionalValueT
    {
        return Get();
    }

    template<class T>
    auto MatrixIndex<T>::Get() const -> OptionalValueT
    {
        return FindValueFromOwner();
    }

    template<class T>
    RelicID MatrixIndex<T>::ID() const
    {
        return id;
    }

    template<class T>
    Reliquary* MatrixIndex<T>::Owner() const
    {
        return owner;
    }

    template<class T>
    auto MatrixIndex<T>::FindValueFromOwner() const -> OptionalValueT
    {
        return MatrixImplementation<T>::CreateIndexValue(id, *owner);
    }

    template<class T>
    struct IndexTypeFor<T, std::enable_if_t<usable_for_matrix_index_v<T>>>
    {
        using Type = MatrixIndex<T>;
    };

    template<class T, std::enable_if_t<usable_for_matrix_index_v<T>, int> = 0>
    MatrixIndex<T> ToIndex(RelicID id, Reliquary& owner)
    {
        return MatrixIndex<T>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::MatrixIndex<T>, BinaryArchive>
        : public CompositeScribe<Arca::MatrixIndex<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::MatrixIndex<T>, BinaryArchive>;
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