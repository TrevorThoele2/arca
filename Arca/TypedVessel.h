#pragma once

#include "VesselID.h"
#include "Reliquary.h"

#include <Chroma/VariadicTemplate.h>
#include <Chroma/Iterate.h>

#include "Serialization.h"

namespace Arca
{
    template<class... Relics>
    class TypedVessel
    {
    public:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
        [[nodiscard]] VesselID ID() const;
    private:
        using VariadicTemplate = Chroma::VariadicTemplate<Relics...>;

        template<::Chroma::VariadicTemplateSize i>
        struct RelicTypeHandle
        {
            using RelicT = typename VariadicTemplate::template Parameter<i - 1>::Type;

            template<class... Args>
            constexpr static VesselStructure Do(Args&& ... typeHandles)
            {
                auto typeHandle = RelicTraits<RelicT>::typeHandle;
                return RelicTypeHandle<i - 1>::Do(typeHandle, typeHandles...);
            }
        };

        template<>
        struct RelicTypeHandle<0>
        {
            template<class... Args>
            constexpr static VesselStructure Do(Args&& ... typeHandles)
            {
                return VesselStructure{ typeHandles... };
            }
        };
    public:
        static inline const VesselStructure structure = RelicTypeHandle<VariadicTemplate::count>::Do();
    protected:
        TypedVessel(VesselID id, Reliquary& owner);
        explicit TypedVessel(const ::Inscription::BinaryTableData<TypedVessel>& data);
    private:
        Reliquary* owner;
        VesselID id;
    private:
        template<::Chroma::VariadicTemplateSize i>
        struct RelicCreator
        {
            using RelicT = typename VariadicTemplate::template Parameter<i - 1>::Type;

            static void Do(TypedVessel& vessel)
            {
                vessel.owner->template CreateRelic<RelicT>(vessel.id);
            }
        };

        template<>
        struct RelicCreator<0>
        {
            static void Do(TypedVessel& vessel)
            {}
        };
    private:
        static_assert(VariadicTemplate::AllUnique::value,
            "All relics inside a TypedVessel must be unique.");
    private:
        friend Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class... Relics>
    Reliquary& TypedVessel<Relics...>::Owner()
    {
        return *owner;
    }

    template<class... Relics>
    const Reliquary& TypedVessel<Relics...>::Owner() const
    {
        return *owner;
    }

    template<class... Relics>
    VesselID TypedVessel<Relics...>::ID() const
    {
        return id;
    }

    template<class... Relics>
    TypedVessel<Relics...>::TypedVessel(VesselID id, Reliquary& owner)
        : owner(&owner), id(id)
    {
        ::Chroma::IterateRange<::Chroma::VariadicTemplateSize, RelicCreator, VariadicTemplate::count>(*this);
    }

    template<class... Relics>
    TypedVessel<Relics...>::TypedVessel(const ::Inscription::BinaryTableData<TypedVessel>& data) :
        owner(data.owner), id(data.id)
    {}
}

namespace Inscription
{
    template<class... Relics>
    struct TableData<::Arca::TypedVessel<Relics...>, BinaryArchive>
        : TableDataBase<::Arca::TypedVessel<Relics...>, BinaryArchive>
    {
        ::Arca::Reliquary* owner;
        ::Arca::VesselID id;
    };

    template<class... Relics>
    class Scribe<::Arca::TypedVessel<Relics...>, BinaryArchive> final :
        public TableScribe<::Arca::TypedVessel<Relics...>, BinaryArchive>
    {
    private:
        using BaseT = TableScribe<::Arca::TypedVessel<Relics...>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    public:
        using TableBase = typename BaseT::TableBase;

        class Table : public TableBase
        {
        public:
            using DataT = typename TableBase::DataT;
        public:
            Table();
        protected:
            using DataLink = typename TableBase::DataLink;
            using TableBase::MergeDataLinks;
        };
    };

    template<class... Relics>
    Scribe<::Arca::TypedVessel<Relics...>, BinaryArchive>::Table::Table()
    {
        MergeDataLinks
        ({
            DataLink::Auto(&ObjectT::owner, &DataT::owner),
            DataLink::Auto(&ObjectT::id, &DataT::id) }
        );
    }
}