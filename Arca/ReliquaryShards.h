#pragma once

#include "ReliquaryComponent.h"

#include <unordered_map>

#include "ShardTraits.h"
#include "ShardBatchSource.h"

#include "RelicID.h"
#include "TypeHandle.h"
#include "Ptr.h"
#include "Created.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class Reliquary;

    class ReliquaryShards : public ReliquaryComponent
    {
    public:
        explicit ReliquaryShards(Reliquary& owner);
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
        using BatchSourcePtr = std::unique_ptr<ShardBatchSourceBase>;
        using BatchSourceMap = std::unordered_map<TypeHandleName, BatchSourcePtr>;
        BatchSourceMap batchSources;
        BatchSourceMap constBatchSources;

        KnownPolymorphicSerializerList serializers;

        [[nodiscard]] ShardBatchSourceBase* FindBatchSource(const TypeHandleName& typeHandle);
        [[nodiscard]] ShardBatchSourceBase* FindConstBatchSource(const TypeHandleName& typeHandle);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] BatchSource<ShardT>* FindBatchSource();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] BatchSource<ShardT>& RequiredBatchSource();

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && !std::is_const_v<ShardT>, int> = 0>
        [[nodiscard]] BatchSourceMap& ShardSourceMapFor();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && std::is_const_v<ShardT>, int> = 0>
        [[nodiscard]] BatchSourceMap& ShardSourceMapFor();
    };
}
