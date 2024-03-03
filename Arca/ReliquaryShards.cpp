#include "ReliquaryShards.h"

#include "Reliquary.h"
#include "ReliquaryRelics.h"
#include "Destroying.h"
#include <cassert>

namespace Arca
{
    void ReliquaryShards::Create(const Type& type, RelicID id)
    {
        const auto factory = factoryMap.find(type.name);
        if (factory == factoryMap.end())
            throw NotRegistered(Type(type.name));

        factory->second(Owner(), id, type.isConst);
        NotifyCompositesShardCreate(id);
    }

    void ReliquaryShards::Destroy(const Handle& handle)
    {
        assert(handle.ObjectType() == HandleObjectType::Shard);

        const auto id = handle.ID();

        Relics().ShardModificationRequired(id);

        {
            for (auto& eitherShardBatchSource : Shards().eitherBatchSources.map)
                eitherShardBatchSource.second->DestroyFromBase(id);
        }

        for (auto& shardBatchSource : Shards().batchSources.map)
        {
            if (shardBatchSource.first != handle.Type().name)
                continue;

            if (shardBatchSource.second->DestroyFromBase(id))
            {
                Owner().Raise<Destroying>(HandleFrom(id, shardBatchSource.second->Type(), HandleObjectType::Shard));
                NotifyCompositesShardDestroy(id);
                break;
            }
        }
    }

    void ReliquaryShards::NotifyCompositesRelicCreate(RelicID id, const RelicStructure& structure)
    {
        for (auto& loop : compositeBatchSources.map)
            loop.second->NotifyRelicCreated(id, structure);
    }

    void ReliquaryShards::NotifyCompositesRelicDestroy(RelicID id)
    {
        for (auto& loop : compositeBatchSources.map)
            loop.second->NotifyRelicDestroyed(id);
    }

    ShardBatchSourceBase* ReliquaryShards::BatchSources::FindConst(const TypeName& typeName)
    {
        const auto found = constMap.find(typeName);
        if (found == constMap.end())
            return nullptr;

        return found->second.get();
    }

    ReliquaryShards::BatchSources::BatchSources(ReliquaryShards& owner) : StorageBatchSources(owner)
    {}

    ReliquaryShards::EitherBatchSources::EitherBatchSources(ReliquaryShards& owner) : MetaBatchSources(owner)
    {}

    ReliquaryShards::CompositeBatchSources::CompositeBatchSources(ReliquaryShards& owner) : MetaBatchSources(owner)
    {}

    void ReliquaryShards::NotifyCompositesShardCreate(RelicID id)
    {
        for (auto& loop : compositeBatchSources.map)
            loop.second->NotifyShardCreated(id);
    }

    void ReliquaryShards::NotifyCompositesShardDestroy(RelicID id)
    {
        for (auto& loop : compositeBatchSources.map)
            loop.second->NotifyShardDestroyed(id);
    }

    ReliquaryShards::ReliquaryShards(Reliquary& owner) : ReliquaryComponent(owner, "shard")
    {}

}
