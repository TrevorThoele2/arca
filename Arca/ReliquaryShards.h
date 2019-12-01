#pragma once

#include "ReliquaryComponent.h"

#include <unordered_map>

#include "ShardTraits.h"
#include "ShardBatchSource.h"

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
        Factory FindFactory();

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
    public:
        class BatchSources : public BatchSourcesBase<ShardBatchSourceBase, ReliquaryShards, BatchSources>
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
            friend BatchSourcesBase<ShardBatchSourceBase, ReliquaryShards, BatchSources>;
        } batchSources = BatchSources(*this);

        KnownPolymorphicSerializerList serializers;
    private:
        explicit ReliquaryShards(Reliquary& owner);
        friend Reliquary;
    };
}
