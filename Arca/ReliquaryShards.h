#pragma once

#include "ReliquaryComponent.h"

#include <unordered_map>

#include "ShardBatch.h"

#include "IsShard.h"

#include "LocalPtr.h"

#include "RelicID.h"
#include "Type.h"
#include <Chroma/VariadicTemplate.h>

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

        [[nodiscard]] bool Contains(const Handle& handle) const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const ShardT& shard) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);
    public:
        class BatchSources
            : public StorageBatchSourcesBase<ShardBatchSourceBase, ReliquaryShards, BatchSources, is_shard>
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

        using Destroyer = std::function<void(Reliquary&, RelicID, bool)>;
        using DestroyerMap = std::unordered_map<TypeName, Destroyer>;
        DestroyerMap destroyerMap;

        KnownPolymorphicSerializerList serializers;
    public:
        ReliquaryShards(const ReliquaryShards& arg) = delete;
        ReliquaryShards& operator=(const ReliquaryShards& arg) = delete;
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