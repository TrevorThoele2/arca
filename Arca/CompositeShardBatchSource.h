#pragma once

#include <vector>

#include "BatchSource.h"
#include "CompositeTraits.h"
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

        [[nodiscard]] virtual SizeT Size() const = 0;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_composite_v<T>>>
        : public CompositeShardBatchSourceBase
    {
    private:
        using Pack = typename T::Pack;

        template<class U>
        struct ToPtr
        {
            using Type = Ptr<U>;
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
        BatchSource(const BatchSource& arg) = delete;
        BatchSource(BatchSource&& arg) = default;

        void Add(RelicID id);

        void NotifyShardCreated(RelicID id) override;
        void NotifyShardDestroyed(RelicID id) override;

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
        void CreateEntry(RelicID id);
        void DestroyEntry(RelicID id);
        [[nodiscard]] bool ContainsEntry(RelicID id) const;
        [[nodiscard]] TupleT CreateTuple(RelicID id);

        template<::Chroma::VariadicTemplateSize i>
        struct CreateTupleIterator
        {
            static void Do(TupleT& tuple, RelicID id, ReliquaryShards& shards);
        };
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}