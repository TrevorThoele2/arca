#pragma once

#include "ReliquaryComponent.h"

#include <unordered_map>

#include "ShardTraits.h"
#include "ShardBatchSource.h"
#include "ShardBatch.h"
#include "EitherShardBatchSource.h"
#include "CompositeShardBatchSource.h"

#include "Ptr.h"

#include "RelicID.h"
#include "Type.h"

namespace Arca
{
    class Reliquary;

    class ReliquaryShards : public ReliquaryComponent
    {
    public:
        using Factory = void(*)(Reliquary&, RelicID, bool);
        using FactoryMap = std::unordered_map<TypeName, Factory>;
        FactoryMap factoryMap;

        void Create(const Type& type, RelicID id);
        template<class ShardT>
        Ptr<ShardT> Create(RelicID id);

        void Destroy(const Handle& handle);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Ptr<ShardT> Find(RelicID id) const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] Ptr<EitherT> Find(RelicID id) const;

        [[nodiscard]] bool Contains(const Handle& handle) const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class... ShardsT, std::enable_if_t<are_all_shards_v<ShardsT...> && (sizeof...(ShardsT) > 1), int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const ShardT& shard) const;

        template<class ShardT>
        void AttemptAddToEitherBatches(RelicID id, ShardT& shard);

        void NotifyCompositesRelicCreate(RelicID id, const RelicStructure& structure);
        void NotifyCompositesRelicDestroy(RelicID id);

        template<class ShardT>
        Factory FindFactory();

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] typename EitherT::ShardT* FindStorage(RelicID id);
    public:
        class BatchSources
            : public StorageBatchSources<ShardBatchSourceBase, ReliquaryShards, BatchSources, is_shard>
        {
        public:
            Map constMap;

            [[nodiscard]] ShardBatchSourceBase* FindConst(const TypeName& typeName);

            template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && !std::is_const_v<ShardT>, int> = 0>
            [[nodiscard]] Map& MapFor();
            template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && !std::is_const_v<ShardT>, int> = 0>
            [[nodiscard]] const Map& MapFor() const;
            template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && std::is_const_v<ShardT>, int> = 0>
            [[nodiscard]] Map& MapFor();
            template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && std::is_const_v<ShardT>, int> = 0>
            [[nodiscard]] const Map& MapFor() const;
        private:
            explicit BatchSources(ReliquaryShards& owner);
            friend ReliquaryShards;
        } batchSources = BatchSources(*this);

        class EitherBatchSources
            : public MetaBatchSources<TypeName, EitherShardBatchSourceBase, ReliquaryShards, EitherBatchSources>
        {
        public:
            template<class T, std::enable_if_t<is_either_v<T>, int> = 0>
            [[nodiscard]] Map& MapFor();
            template<class T, std::enable_if_t<is_either_v<T>, int> = 0>
            [[nodiscard]] const Map& MapFor() const;
        private:
            explicit EitherBatchSources(ReliquaryShards& owner);
            friend ReliquaryShards;
        private:
            template<class T>
            using ShardT = typename T::BareT;

            template<class T>
            std::unique_ptr<EitherShardBatchSourceBase> Create();

            template<class T>
            [[nodiscard]] static TypeName KeyFor();

            template<class T>
            constexpr static bool should_accept = is_either_v<T>;
            friend MetaBatchSources<TypeName, EitherShardBatchSourceBase, ReliquaryShards, EitherBatchSources>;
        } eitherBatchSources = EitherBatchSources(*this);

        class CompositeBatchSources
            : public MetaBatchSources<
                std::type_index,
                CompositeShardBatchSourceBase,
                ReliquaryShards,
                CompositeBatchSources>
        {
        public:
            template<class T, std::enable_if_t<is_composite_v<T>, int> = 0>
            [[nodiscard]] Map& MapFor();
            template<class T, std::enable_if_t<is_composite_v<T>, int> = 0>
            [[nodiscard]] const Map& MapFor() const;
        private:
            explicit CompositeBatchSources(ReliquaryShards& owner);
            friend ReliquaryShards;
        private:
            template<class T>
            std::unique_ptr<CompositeShardBatchSourceBase> Create();

            template<class T>
            [[nodiscard]] static std::type_index KeyFor();

            template<class T>
            constexpr static bool should_accept = is_composite_v<T>;
            friend MetaBatchSources<std::type_index, CompositeShardBatchSourceBase, ReliquaryShards, CompositeBatchSources>;
        } compositeBatchSources = CompositeBatchSources(*this);

        KnownPolymorphicSerializerList serializers;
    public:
        ReliquaryShards(const ReliquaryShards& arg) = delete;
        ReliquaryShards& operator=(const ReliquaryShards& arg) = delete;
    private:
        [[nodiscard]] bool HasEitherType(Type type, RelicID id) const;
    private:
        void NotifyCompositesShardCreate(RelicID id);
        void NotifyCompositesShardDestroy(RelicID id);
    private:
        template<class ShardT>
        Ptr<ShardT> CreatePtr(RelicID id) const;
    private:
        template<::Chroma::VariadicTemplateSize i>
        struct ContainsAllShardsIterator
        {
            template<class ShardPack>
            static bool Check(ShardPack, RelicID id, const ReliquaryShards& shards)
            {
                using T = typename ShardPack::template Parameter<i>::Type;

                return shards.Contains<T>(id);
            }
        };
    private:
        explicit ReliquaryShards(Reliquary& owner);
        ReliquaryShards(ReliquaryShards&& arg) noexcept = default;
        friend Reliquary;
    };
}

#include "EitherShardBatchSourceDefinition.h"