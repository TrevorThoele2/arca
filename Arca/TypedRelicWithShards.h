#pragma once

#include "RelicID.h"
#include "Reliquary.h"

#include <Chroma/VariadicTemplate.h>

#include "Serialization.h"

namespace Arca
{
    template<class... Shards>
    class TypedRelicWithShards
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
        TypedRelicWithShards(RelicID id, Reliquary& owner);
        explicit TypedRelicWithShards(const ::Inscription::BinaryTableData<TypedRelicWithShards>& data);

        void ExtractShards(const ShardTuple& shards) const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
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
            "All shards inside a TypedRelicWithShards must be unique.");
    private:
        friend Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class... Shards>
    Reliquary& TypedRelicWithShards<Shards...>::Owner()
    {
        return *owner;
    }

    template<class... Shards>
    const Reliquary& TypedRelicWithShards<Shards...>::Owner() const
    {
        return *owner;
    }

    template<class... Shards>
    RelicID TypedRelicWithShards<Shards...>::ID() const
    {
        return id;
    }

    template<class... Shards>
    const RelicStructure TypedRelicWithShards<Shards...>::structure = ShardTypeHandle<VariadicTemplate::count>::Do();

    template<class... Shards>
    TypedRelicWithShards<Shards...>::TypedRelicWithShards(RelicID id, Reliquary& owner) : id(id), owner(&owner)
    {}

    template<class... Shards>
    TypedRelicWithShards<Shards...>::TypedRelicWithShards(
        const ::Inscription::BinaryTableData<TypedRelicWithShards>& data)
        :
        id(data.id), owner(data.owner)
    {}

    template<class... Shards>
    void TypedRelicWithShards<Shards...>::ExtractShards(const ShardTuple& shards) const
    {
        ShardExtraction<VariadicTemplate::count>::Do(*owner, id, shards);
    }
}

namespace Inscription
{
    template<class... Shards>
    struct TableData<::Arca::TypedRelicWithShards<Shards...>, BinaryArchive> final :
        TableDataBase<::Arca::TypedRelicWithShards<Shards...>, BinaryArchive>
    {
        ::Arca::RelicID id;
        ::Arca::Reliquary* owner;
    };

    template<class... Shards>
    class Scribe<::Arca::TypedRelicWithShards<Shards...>, BinaryArchive> final :
        public TableScribe<::Arca::TypedRelicWithShards<Shards...>, BinaryArchive>
    {
    private:
        using BaseT = TableScribe<::Arca::TypedRelicWithShards<Shards...>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        using TableBase = typename BaseT::TableBase;
    public:
        class Table : public TableBase
        {
        public:
            using DataT = typename TableBase::DataT;

            Table()
            {
                MergeDataLinks({
                    DataLink::Auto(&ObjectT::id, &DataT::id),
                    DataLink::Auto(&ObjectT::owner, &DataT::owner) }
                );
            }
        protected:
            using DataLink = typename TableBase::DataLink;
            using TableBase::MergeDataLinks;
        };
    };
}