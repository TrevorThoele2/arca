#pragma once

#include "RelicID.h"
#include "Reliquary.h"

#include <Chroma/VariadicTemplate.h>

#include "Serialization.h"

namespace Arca
{
    template<class... Shards>
    class TypedRelic
    {
    public:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
        [[nodiscard]] RelicID ID() const;
    private:
        using VariadicTemplate = Chroma::VariadicTemplate<Shards...>;

        template<::Chroma::VariadicTemplateSize i>
        struct ShardTypeHandle
        {
            using ShardT = typename VariadicTemplate::template Parameter<i - 1>::Type;

            template<class... Args>
            constexpr static RelicStructure Do(Args&& ... typeHandles)
            {
                auto typeHandle = ShardTraits<ShardT>::typeHandle;
                return ShardTypeHandle<i - 1>::Do(typeHandle, typeHandles...);
            }
        };

        template<>
        struct ShardTypeHandle<0>
        {
            template<class... Args>
            constexpr static RelicStructure Do(Args&& ... typeHandles)
            {
                return RelicStructure{ typeHandles... };
            }
        };
    private:
        template<class T>
        struct ShardTransformer
        {
            using Type = T*&;
        };
    protected:
        using ShardTuple = typename VariadicTemplate::template Transform<ShardTransformer>::Type::TupleT;
    public:
        static const RelicStructure structure;
    protected:
        TypedRelic(RelicID id, Reliquary& owner);
        explicit TypedRelic(const ::Inscription::BinaryTableData<TypedRelic>& data);

        void ExtractShards(const ShardTuple& shards) const;
    private:
        Reliquary* owner;
        RelicID id;
    private:
        template<::Chroma::VariadicTemplateSize i>
        struct ShardExtraction
        {
            using ShardT = typename VariadicTemplate::template Parameter<i - 1>::Type;

            template<class... Args>
            static void Do(Reliquary& reliquary, RelicID id, const ShardTuple& shards)
            {
                auto retrieved = reliquary.FindShard<ShardT>(id);
                std::get<i - 1>(shards) = retrieved;
                ShardExtraction<i - 1>::Do(reliquary, id, shards);
            }
        };

        template<>
        struct ShardExtraction<0>
        {
            template<class... Args>
            static void Do(Reliquary&, RelicID, const ShardTuple&)
            {}
        };
    private:
        static_assert(VariadicTemplate::AllUnique::value,
            "All shards inside a TypedRelic must be unique.");
    private:
        friend Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class... Shards>
    Reliquary& TypedRelic<Shards...>::Owner()
    {
        return *owner;
    }

    template<class... Shards>
    const Reliquary& TypedRelic<Shards...>::Owner() const
    {
        return *owner;
    }

    template<class... Shards>
    RelicID TypedRelic<Shards...>::ID() const
    {
        return id;
    }

    template<class... Shards>
    const RelicStructure TypedRelic<Shards...>::structure = ShardTypeHandle<VariadicTemplate::count>::Do();

    template<class... Shards>
    TypedRelic<Shards...>::TypedRelic(RelicID id, Reliquary& owner)
        : owner(&owner), id(id)
    {}

    template<class... Shards>
    TypedRelic<Shards...>::TypedRelic(const ::Inscription::BinaryTableData<TypedRelic>& data) :
        owner(data.owner), id(data.id)
    {}

    template<class... Shards>
    void TypedRelic<Shards...>::ExtractShards(const ShardTuple& shards) const
    {
        ShardExtraction<VariadicTemplate::count>::Do(*owner, id, shards);
    }
}

namespace Inscription
{
    template<class... Shards>
    struct TableData<::Arca::TypedRelic<Shards...>, BinaryArchive>
        : TableDataBase<::Arca::TypedRelic<Shards...>, BinaryArchive>
    {
        ::Arca::Reliquary* owner;
        ::Arca::RelicID id;
    };

    template<class... Shards>
    class Scribe<::Arca::TypedRelic<Shards...>, BinaryArchive> final :
        public TableScribe<::Arca::TypedRelic<Shards...>, BinaryArchive>
    {
    private:
        using BaseT = TableScribe<::Arca::TypedRelic<Shards...>, BinaryArchive>;
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

    template<class... Shards>
    Scribe<::Arca::TypedRelic<Shards...>, BinaryArchive>::Table::Table()
    {
        MergeDataLinks
        ({
            DataLink::Auto(&ObjectT::owner, &DataT::owner),
            DataLink::Auto(&ObjectT::id, &DataT::id) }
        );
    }
}