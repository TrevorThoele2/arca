#pragma once

#include <vector>

#include "BatchSource.h"

#include "RelicID.h"
#include "Either.h"

#include "Serialization.h"

namespace Arca
{
    class EitherShardBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~EitherShardBatchSourceBase() = 0;

        virtual void DestroyFromBase(RelicID id, bool isConst) = 0;
        virtual void DestroyFromBase(RelicID id) = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;
    };

    class ReliquaryShards;

    template<class T>
    class BatchSource<T, std::enable_if_t<is_either_v<T>>> : public EitherShardBatchSourceBase
    {
    public:
        using ShardT = typename T::ShardT;
    private:
        struct Entry
        {
            RelicID id;
            const ShardT* shard;
            bool isConst;

            Entry(RelicID id, const ShardT& shard, bool isConst);
        };

        using List = std::vector<Entry>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        explicit BatchSource(ReliquaryShards& owner);
        BatchSource(const BatchSource& arg) = delete;
        BatchSource(BatchSource&& arg) = default;

        void Add(RelicID id, const ShardT& add, bool isConst);

        iterator Destroy(RelicID destroy, bool isConst);
        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        void DestroyFromBase(RelicID id, bool isConst) override;
        void DestroyFromBase(RelicID id) override;

        const ShardT* Find(RelicID id, bool isConst);
        const ShardT* Find(RelicID id);

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
    private:
        ReliquaryShards* owner;
        size_t referenceCount = 0;
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}