#include "ReliquaryShards.h"

namespace Arca
{
    ReliquaryShards::ReliquaryShards(Reliquary& owner) : ReliquaryComponent(owner, "shard")
    {}

    void ReliquaryShards::Create(const TypeHandle& typeHandle, RelicID id)
    {
        const auto factory = factoryMap.find(typeHandle.name);
        if (factory == factoryMap.end())
            throw NotRegistered(typeHandle.name);

        factory->second(Owner(), id, typeHandle.isConst);
    }

    ShardBatchSourceBase* ReliquaryShards::FindBatchSource(const TypeHandleName& typeHandle)
    {
        const auto found = batchSources.find(typeHandle);
        if (found == batchSources.end())
            return nullptr;

        return found->second.get();
    }

    ShardBatchSourceBase* ReliquaryShards::FindConstBatchSource(const TypeHandleName& typeHandle)
    {
        const auto found = constBatchSources.find(typeHandle);
        if (found == constBatchSources.end())
            return nullptr;

        return found->second.get();
    }
}
