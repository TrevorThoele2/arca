#pragma once

#include "ReliquaryComponent.h"

#include <unordered_map>

#include "ShardTraits.h"
#include "ShardBatchSource.h"
#include "EitherShardBatchSource.h"

#include "RelicID.h"
#include "TypeHandle.h"
#include "Ptr.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class Reliquary;

    class ReliquaryShards : public ReliquaryComponent
    {
    public:
        using Factory = void(*)(Reliquary&, RelicID, bool);
        using FactoryMap = std::unordered_map<TypeHandleName, Factory>;
        FactoryMap factoryMap;

        void Create(const TypeHandle& typeHandle, RelicID id);
        template<class ShardT>
        Ptr<ShardT> Create(RelicID id);
        template<class ShardT>
        void Destroy(RelicID id);

        template<class ShardT>
        void AttemptAddToEitherBatches(RelicID id, ShardT& shard);

        template<class ShardT>
        Factory FindFactory();

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
    public:
        class BatchSources : public StorageBatchSources<ShardBatchSourceBase, ReliquaryShards, BatchSources>
        {
        public:
            Map constMap;

            [[nodiscard]] ShardBatchSourceBase* FindConst(const TypeHandleName& typeHandle);

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
        private:
            template<class ShardT>
            constexpr static bool is_object_v = is_shard_v<ShardT>;
            friend StorageBatchSources<ShardBatchSourceBase, ReliquaryShards, BatchSources>;
        } batchSources = BatchSources(*this);

        class EitherBatchSources
            : public MetaBatchSources<EitherShardBatchSourceBase, ReliquaryShards, EitherBatchSources>
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
            std::unique_ptr<EitherShardBatchSourceBase> Create()
            {
                auto created = std::make_unique<BatchSource<T>>(*owner);
                for (auto& loop : owner->batchSources.Required<ShardT<T>>())
                    created->Add(loop.id, loop.shard, false);
                for (auto& loop : owner->batchSources.Required<const ShardT<T>>())
                    created->Add(loop.id, loop.shard, true);
                return created;
            }

            template<class T>
            [[nodiscard]] static TypeHandle TypeHandleFor()
            {
                return Arca::TypeHandleFor<ShardT<T>>();
            }

            template<class T>
            constexpr static bool is_object_v = is_either_v<T>;
            friend MetaBatchSources<EitherShardBatchSourceBase, ReliquaryShards, EitherBatchSources>;
        } eitherBatchSources = EitherBatchSources(*this);

        KnownPolymorphicSerializerList serializers;
    private:
        explicit ReliquaryShards(Reliquary& owner);
        friend Reliquary;
    };
}

#include "EitherShardBatchSourceDefinition.h"