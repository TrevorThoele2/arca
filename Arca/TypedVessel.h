#pragma once

#include "VesselID.h"
#include "Reliquary.h"

#include <Chroma/VariadicTemplate.h>

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
    private:
        template<class T>
        struct RelicTransformer
        {
            using Type = T*&;
        };
    protected:
        using RelicTuple = typename VariadicTemplate::template Transform<RelicTransformer>::Type::TupleT;
    public:
        static const VesselStructure structure;
    protected:
        TypedVessel(VesselID id, Reliquary& owner);
        explicit TypedVessel(const ::Inscription::BinaryTableData<TypedVessel>& data);

        void ExtractRelics(const RelicTuple& relics) const;
    private:
        Reliquary* owner;
        VesselID id;
    private:
        template<::Chroma::VariadicTemplateSize i>
        struct RelicExtraction
        {
            using RelicT = typename VariadicTemplate::template Parameter<i - 1>::Type;

            template<class... Args>
            static void Do(Reliquary& reliquary, VesselID id, const RelicTuple& relics)
            {
                auto retrieved = reliquary.FindRelic<RelicT>(id);
                std::get<i - 1>(relics) = retrieved;
                RelicExtraction<i - 1>::Do(reliquary, id, relics);
            }
        };

        template<>
        struct RelicExtraction<0>
        {
            template<class... Args>
            static void Do(Reliquary&, VesselID, const RelicTuple&)
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
    const VesselStructure TypedVessel<Relics...>::structure = RelicTypeHandle<VariadicTemplate::count>::Do();

    template<class... Relics>
    TypedVessel<Relics...>::TypedVessel(VesselID id, Reliquary& owner)
        : owner(&owner), id(id)
    {}

    template<class... Relics>
    TypedVessel<Relics...>::TypedVessel(const ::Inscription::BinaryTableData<TypedVessel>& data) :
        owner(data.owner), id(data.id)
    {}

    template<class... Relics>
    void TypedVessel<Relics...>::ExtractRelics(const RelicTuple& relics) const
    {
        RelicExtraction<VariadicTemplate::count>::Do(*owner, id, relics);
    }
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