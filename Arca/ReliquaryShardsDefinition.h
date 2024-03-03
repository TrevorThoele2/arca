#pragma once

#include "ReliquaryShards.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT>
    LocalPtr<ShardT> ReliquaryShards::Create(RelicID id)
    {
        Relics().ShardModificationRequired(id);

        const auto type = TypeFor<ShardT>();
        if (Contains(Handle(id, Owner(), type, HandleObjectType::Shard)))
            throw CannotCreate(type);

        auto& batchSource = batchSources.Required<ShardT>();
        auto added = batchSource.Add(id);

        Owner().matrices.NotifyCreated(id);
        Owner().Raise<Created>(HandleFrom(id, type, HandleObjectType::Shard));
        Owner().Raise<CreatedKnown<ShardT>>(CreatePtr<ShardT>(id));

        return CreatePtr<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void ReliquaryShards::Destroy(RelicID id)
    {
        Relics().ShardModificationRequired(id);

        auto& map = batchSources.MapFor<ShardT>();
        auto shardBatchSource = map.find(TypeFor<ShardT>().name);
        if (shardBatchSource != map.end())
        {
            if (shardBatchSource->second->DestroyFromBase(id))
            {
                Owner().matrices.NotifyDestroying(id);
                Owner().Raise<DestroyingKnown<ShardT>>(CreatePtr<ShardT>(id));
                Owner().Raise<Destroying>(HandleFrom(id, shardBatchSource->second->Type(), HandleObjectType::Shard));
            }
        }
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        auto& batchSource = batchSources.Required<ShardT>();
        auto found = batchSource.Find(id);
        return static_cast<bool>(found);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    RelicID ReliquaryShards::IDFor(const ShardT& shard) const
    {
        auto& batchSource = batchSources.Required<ShardT>();
        for (auto loop = batchSource.begin(); loop != batchSource.end(); ++loop)
            if (&loop->shard == &shard)
                return loop->id;

        return 0;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* ReliquaryShards::FindStorage(RelicID id)
    {
        auto& batchSource = batchSources.Required<ShardT>();
        return batchSource.Find(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && !std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::BatchSources::MapFor() -> Map&
    {
        return map;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && !std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::BatchSources::MapFor() const -> const Map&
    {
        return map;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::BatchSources::MapFor() -> Map&
    {
        return constMap;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT> && std::is_const_v<ShardT>, int>>
    auto ReliquaryShards::BatchSources::MapFor() const -> const Map&
    {
        return constMap;
    }

    template<class T>
    auto ReliquaryShards::CreatePtr(RelicID id) const
    {
        return ToPtr<T>(id, const_cast<Reliquary&>(Owner()));
    }
}