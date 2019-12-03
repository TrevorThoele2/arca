#include "ReliquaryShards.h"

namespace Arca
{
    void ReliquaryShards::Create(const TypeHandle& typeHandle, RelicID id)
    {
        const auto factory = factoryMap.find(typeHandle.name);
        if (factory == factoryMap.end())
            throw NotRegistered(typeHandle.name);

        factory->second(Owner(), id, typeHandle.isConst);
    }

    ShardBatchSourceBase* ReliquaryShards::BatchSources::FindConst(const TypeHandleName& typeHandle)
    {
        const auto found = constMap.find(typeHandle);
        if (found == constMap.end())
            return nullptr;

        return found->second.get();
    }

    ReliquaryShards::BatchSources::BatchSources(ReliquaryShards& owner) : StorageBatchSources(owner)
    {}

    ReliquaryShards::EitherBatchSources::EitherBatchSources(ReliquaryShards& owner) : MetaBatchSources(owner)
    {}

    ReliquaryShards::ReliquaryShards(Reliquary& owner) : ReliquaryComponent(owner, "shard")
    {}

}
