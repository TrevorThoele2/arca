#include "ReliquaryShards.h"

#include "Reliquary.h"
#include "ReliquaryRelics.h"
#include <cassert>
#include <utility>

namespace Arca
{
    void ReliquaryShards::Create(const Type& type, RelicID id)
    {
        const auto factory = factoryMap.find(type.name);
        if (factory == factoryMap.end())
            throw NotRegistered(Type(type.name));

        if (Contains(Handle(id, Owner(), type, HandleObjectType::Shard)))
            throw CannotCreate(type);

        factory->second(Owner(), id, type.isConst);
    }

    void ReliquaryShards::Destroy(const Type& type, RelicID id)
    {
        const auto destroyer = destroyerMap.find(type.name);
        if (destroyer == destroyerMap.end())
            throw NotRegistered(type);

        destroyer->second(Owner(), id, type.isConst);
    }

    bool ReliquaryShards::Contains(const Handle& handle) const
    {
        assert(handle.ObjectType() == HandleObjectType::Shard);

        const auto id = handle.ID();

        const auto shardBatchSource = batchSources.map.find(handle.Type().name);
        if (shardBatchSource != batchSources.map.end())
            return shardBatchSource->second->ContainsFromBase(id);

        return false;
    }

    ShardBatchSourceBase* ReliquaryShards::BatchSources::FindConst(const TypeName& typeName)
    {
        const auto found = constMap.find(typeName);
        if (found == constMap.end())
            return nullptr;

        return found->second.get();
    }

    ReliquaryShards::BatchSources::BatchSources(ReliquaryShards& owner) :
        StorageBatchSourcesBase(owner)
    {}

    ReliquaryShards::ReliquaryShards(Reliquary& owner) : ReliquaryComponent(owner, "shard")
    {}
}
