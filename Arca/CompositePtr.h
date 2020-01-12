#pragma once

#include "RelicID.h"

#include "Composite.h"
#include "UsableForCompositePtr.h"

#include "PtrTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    class CompositePtr
    {
    private:
        using PackT = typename T::Pack;
    public:
        using ValueT = typename T::Pack::template Transform<PtrTypeFor>::Type::TupleT;
    public:
        CompositePtr() = default;

        CompositePtr(RelicID id, Reliquary& owner) : id(id), owner(&owner)
        {}

        CompositePtr(const CompositePtr& arg) : id(arg.id), owner(arg.owner)
        {}

        CompositePtr(CompositePtr&& arg) noexcept : id(arg.id), owner(arg.owner)
        {}

        CompositePtr& operator=(const CompositePtr& arg)
        {
            id = arg.id;
            owner = arg.owner;
            return *this;
        }

        CompositePtr& operator=(CompositePtr&& arg) noexcept
        {
            id = arg.id;
            owner = arg.owner;
            return *this;
        }

        bool operator==(const CompositePtr& arg) const
        {
            return id == arg.id && owner == arg.owner;
        }

        bool operator!=(const CompositePtr& arg) const
        {
            return !(*this == arg);
        }

        explicit operator bool() const
        {
            return static_cast<bool>(Get());
        }

        operator std::optional<ValueT> () const
        {
            return Get();
        }

        [[nodiscard]] ValueT operator*() const
        {
            return *Get();
        }

        [[nodiscard]] std::optional<ValueT> operator->() const
        {
            return Get();
        }

        [[nodiscard]] std::optional<ValueT> Get() const
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
        [[nodiscard]] std::optional<ValueT> FindValueFromOwner() const
        {
            ValueT tuple;

            ::Chroma::IterateRange<::Chroma::VariadicTemplateSize, CompositeToTuple, PackT::count - 1>
                (tuple, id, *owner);

            if (
                ::Chroma::IterateRangeCheckStop<
                ::Chroma::VariadicTemplateSize,
                CompositeTupleContainsValid,
                bool,
                PackT::count - 1>
                (false, tuple))
                return tuple;
            else
                return {};
        }
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    struct PtrTypeFor<T, std::enable_if_t<usable_for_composite_ptr_v<T>>>
    {
        using Type = CompositePtr<T>;
    };

    template<class T, std::enable_if_t<usable_for_composite_ptr_v<T>, int> = 0>
    CompositePtr<T> ToPtr(RelicID id, Reliquary& owner)
    {
        return CompositePtr<T>(id, owner);
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::CompositePtr<T>, BinaryArchive>
        : public CompositeScribe<Arca::CompositePtr<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::CompositePtr<T>, BinaryArchive>;
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