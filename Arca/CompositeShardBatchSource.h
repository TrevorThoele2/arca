#pragma once

#include <vector>

#include "BatchSource.h"
#include "AreAllShards.h"
#include "All.h"
#include "Ptr.h"
#include "RelicStructure.h"

namespace Arca
{
    class ReliquaryShards;

    class CompositeShardBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~CompositeShardBatchSourceBase() = 0;

        virtual void NotifyShardCreated(RelicID id) = 0;
        virtual void NotifyShardDestroyed(RelicID id) = 0;
        virtual void NotifyRelicCreated(RelicID id, const RelicStructure& structure) = 0;
        virtual void NotifyRelicDestroyed(RelicID id) = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;
    };

    template<class ShardsT, class = void>
    struct is_composite : std::false_type
    {};

    template<class ShardsT>
    struct is_composite<ShardsT, std::enable_if_t<
        is_all_v<ShardsT>
        && ShardsT::Pack::template ForwardArguments<are_all_shards>::Type::value
        && (ShardsT::Pack::count > 1)>>
        : std::true_type
    {};

    template<class ShardsT>
    static constexpr bool is_composite_v = is_composite<ShardsT>::value;

    template<class T>
    class BatchSource<T, std::enable_if_t<is_composite_v<T>>>
        : public CompositeShardBatchSourceBase
    {
    private:
        using Pack = typename T::Pack;

        template<class U>
        struct ToPtr
        {
            using Type = LocalPtr<U>;
        };
    public:
        using TupleT = typename Pack::template Transform<ToPtr>::Type::TupleT;
    private:
        struct Entry
        {
            RelicID id;
            TupleT tuple;
        };

        using List = std::vector<Entry>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        explicit BatchSource(ReliquaryShards& owner);

        void Add(RelicID id);

        void NotifyShardCreated(RelicID id) override;
        void NotifyShardDestroyed(RelicID id) override;
        void NotifyRelicCreated(RelicID id, const RelicStructure& structure) override;
        void NotifyRelicDestroyed(RelicID id) override;

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    public:
        void Reference();
        void Dereference();
    private:
        List list;
        ReliquaryShards* owner;
        size_t referenceCount = 0;
    private:
        RelicStructure structure;
        bool StructureContains(TypeHandle typeHandle) const;
    private:
        void CreateEntry(RelicID id);
        void DestroyEntry(RelicID id);
        [[nodiscard]] bool ContainsEntry(RelicID id) const;
        [[nodiscard]] TupleT CreateTuple(RelicID id);

        template<::Chroma::VariadicTemplateSize i>
        struct CreateTupleIterator
        {
            static void Do(TupleT& tuple, RelicID id, ReliquaryShards& shards);
        };

        template<::Chroma::VariadicTemplateSize i>
        struct ToStructure
        {
            static void Do(RelicStructure& structure);
        };
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}