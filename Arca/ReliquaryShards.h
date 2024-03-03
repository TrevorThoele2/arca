#pragma once

#include "ReliquaryComponent.h"

#include <unordered_map>

#include "ShardBatch.h"
#include "EitherShardBatch.h"
#include "CompositeShardBatch.h"

#include "IsShard.h"
#include "IsEither.h"
#include "IsComposite.h"

#include "LocalPtr.h"

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
        LocalPtr<ShardT> Create(RelicID id);

        void Destroy(const Type& type, RelicID id);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy(RelicID id);
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        void Destroy(RelicID id);
        template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int> = 0>
        void Destroy(RelicID id);

        [[nodiscard]] bool Contains(const Handle& handle) const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const ShardT& shard) const;

        template<class ShardT>
        void AttemptAddToEitherBatches(RelicID id, ShardT& shard);

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
            : public MetaBatchSources<
                TypeName,
                EitherShardBatchSourceBase,
                ReliquaryShards,
                EitherBatchSources,
                is_either>
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
            friend MetaBatchSources<TypeName, EitherShardBatchSourceBase, ReliquaryShards, EitherBatchSources, is_either>;
        } eitherBatchSources = EitherBatchSources(*this);

        class CompositeBatchSources
            : public MetaBatchSources<
                std::type_index,
                CompositeShardBatchSourceBase,
                ReliquaryShards,
                CompositeBatchSources,
                is_composite>
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
            friend MetaBatchSources<std::type_index, CompositeShardBatchSourceBase, ReliquaryShards, CompositeBatchSources, is_composite>;
        } compositeBatchSources = CompositeBatchSources(*this);

        using Destroyer = std::function<void(Reliquary&, RelicID, bool)>;
        using DestroyerMap = std::unordered_map<TypeName, Destroyer>;
        DestroyerMap destroyerMap;

        KnownPolymorphicSerializerList serializers;

        void ClearMetaBatchSources();
    public:
        ReliquaryShards(const ReliquaryShards& arg) = delete;
        ReliquaryShards& operator=(const ReliquaryShards& arg) = delete;
    private:
        [[nodiscard]] bool HasEitherType(Type type, RelicID id) const;
    private:
        void NotifyCompositesShardCreate(RelicID id);
        void NotifyCompositesShardDestroy(RelicID id);
    private:
        template<class T>
        [[nodiscard]] auto CreatePtr(RelicID id) const;
    private:
        template<::Chroma::VariadicTemplateSize i>
        struct DestroyAllShardsIterator
        {
            template<class ShardPack>
            static void Do(ShardPack, RelicID id, ReliquaryShards& shards)
            {
                using T = typename ShardPack::template Parameter<i>::Type;

                shards.Destroy<T>(id);
            }
        };

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