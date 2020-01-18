#pragma once

#include "RelicID.h"

#include "MatrixImplementation.h"
#include "UsableForMatrixPtr.h"

#include "PtrTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class MatrixPtr
    {
    public:
        using ReferenceValueT = typename MatrixImplementation<T>::PtrReference;
        using OptionalValueT = typename MatrixImplementation<T>::PtrOptional;
    public:
        MatrixPtr() = default;

        MatrixPtr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
        {}

        MatrixPtr(const MatrixPtr& arg) : id(arg.id), owner(arg.owner)
        {}

        MatrixPtr(MatrixPtr&& arg) noexcept : id(arg.id), owner(arg.owner)
        {}

        MatrixPtr& operator=(const MatrixPtr& arg)
        {
            id = arg.id;
            owner = arg.owner;
            return *this;
        }

        MatrixPtr& operator=(MatrixPtr&& arg) noexcept
        {
            id = arg.id;
            owner = arg.owner;
            return *this;
        }

        bool operator==(const MatrixPtr& arg) const
        {
            return id == arg.id && owner == arg.owner;
        }

        bool operator!=(const MatrixPtr& arg) const
        {
            return !(*this == arg);
        }

        explicit operator bool() const
        {
            return static_cast<bool>(Get());
        }

        operator OptionalValueT() const
        {
            return Get();
        }

        [[nodiscard]] ReferenceValueT operator*() const
        {
            return *Get();
        }

        [[nodiscard]] OptionalValueT operator->() const
        {
            return Get();
        }

        [[nodiscard]] OptionalValueT Get() const
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
        [[nodiscard]] OptionalValueT FindValueFromOwner() const
        {
            return MatrixImplementation<T>::CreatePtrValue(id , *owner);
        }
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    struct PtrTypeFor<T, std::enable_if_t<usable_for_matrix_ptr_v<T>>>
    {
        using Type = MatrixPtr<T>;
    };

    template<class T, std::enable_if_t<usable_for_matrix_ptr_v<T>, int> = 0>
    MatrixPtr<T> ToPtr(RelicID id, Reliquary& owner)
    {
        return MatrixPtr<T>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::MatrixPtr<T>, BinaryArchive>
        : public CompositeScribe<Arca::MatrixPtr<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::MatrixPtr<T>, BinaryArchive>;
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