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
        if (HasEitherType(type, id))
            throw CannotCreate(type);

        auto& batchSource = batchSources.Required<ShardT>();
        auto added = batchSource.Add(id);

        AttemptAddToEitherBatches(id, *added);
        NotifyCompositesShardCreate(id);

        Owner().Raise<Created>(HandleFrom(id, type, HandleObjectType::Shard));
        return CreatePtr<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        auto& batchSource = batchSources.Required<ShardT>();
        auto found = batchSource.Find(id);
        return static_cast<bool>(found);
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        using ShardT = typename EitherT::BareT;
        return Contains<ShardT>(id) || Contains<const ShardT>(id);
    }

    template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int>>
    bool ReliquaryShards::Contains(RelicID id) const
    {
        return ::Chroma::IterateRangeCheckStop<
            ::Chroma::VariadicTemplateSize,
            ContainsAllShardsIterator,
            bool,
            ShardsT::Pack::count - 1>

            (false, typename ShardsT::Pack{}, id, *this);
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

    template<class ShardT>
    void ReliquaryShards::AttemptAddToEitherBatches(RelicID id, ShardT& shard)
    {
        auto found = eitherBatchSources.Find<Either<ShardT>>();
        if (found)
            found->Add(id, shard, std::is_const_v<ShardT>);
    }

    template<class ShardT>
    auto ReliquaryShards::FindFactory() -> Factory
    {
        auto type = TypeFor<ShardT>();
        const auto found = factoryMap.find(type);
        if (found == factoryMap.end())
            return nullptr;

        return found->second;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* ReliquaryShards::FindStorage(RelicID id)
    {
        auto& batchSource = batchSources.Required<ShardT>();
        return batchSource.Find(id);
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int>>
    typename EitherT::ShardT* ReliquaryShards::FindStorage(RelicID id)
    {
        using ShardT = typename EitherT::BareT;

        {
            auto& batchSource = batchSources.Required<ShardT>();
            auto found = batchSource.Find(id);
            if (found)
                return found;
        }

        {
            auto& batchSource = batchSources.Required<const ShardT>();
            auto found = batchSource.Find(id);
            if (found)
                return found;
        }

        return nullptr;
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

    template<class T, std::enable_if_t<is_either_v<T>, int>>
    auto ReliquaryShards::EitherBatchSources::MapFor() -> Map&
    {
        return map;
    }

    template<class T, std::enable_if_t<is_either_v<T>, int>>
    auto ReliquaryShards::EitherBatchSources::MapFor() const -> const Map&
    {
        return map;
    }

    template<class T>
    std::unique_ptr<EitherShardBatchSourceBase> ReliquaryShards::EitherBatchSources::Create()
    {
        auto created = std::make_unique<BatchSource<T>>(*owner);
        auto& nonConstBatchSource = owner->batchSources.Required<ShardT<T>>();
        for (auto& loop : nonConstBatchSource)
            created->Add(loop.id, loop.shard, false);
        auto& constBatchSource = owner->batchSources.Required<const ShardT<T>>();
        for (auto& loop : constBatchSource)
            created->Add(loop.id, loop.shard, true);
        return created;
    }

    template<class T>
    TypeName ReliquaryShards::EitherBatchSources::KeyFor()
    {
        return Arca::TypeFor<ShardT<T>>().name;
    }

    template<class T, std::enable_if_t<is_composite_v<T>, int>>
    auto ReliquaryShards::CompositeBatchSources::MapFor() -> Map&
    {
        return map;
    }

    template<class T, std::enable_if_t<is_composite_v<T>, int>>
    auto ReliquaryShards::CompositeBatchSources::MapFor() const -> const Map&
    {
        return map;
    }

    template<class T>
    std::unique_ptr<CompositeShardBatchSourceBase> ReliquaryShards::CompositeBatchSources::Create()
    {
        auto created = std::make_unique<BatchSource<T>>(*owner);
        auto allIDs = owner->Owner().AllIDs();
        for (auto& loop : allIDs)
            if (owner->Owner().Contains<T>(loop))
                created->Add(loop);
        return created;
    }

    template<class T>
    [[nodiscard]] std::type_index ReliquaryShards::CompositeBatchSources::KeyFor()
    {
        return std::type_index(typeid(T));
    }

    template<class ShardT>
    LocalPtr<ShardT> ReliquaryShards::CreatePtr(RelicID id) const
    {
        return { id, const_cast<Reliquary&>(Owner()) };
    }
}