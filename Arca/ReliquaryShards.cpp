#include "ReliquaryShards.h"

#include "Reliquary.h"
#include "ReliquaryRelics.h"
#include <cassert>

namespace Arca
{
    void ReliquaryShards::Create(const Type& type, RelicID id)
    {
        const auto handler = FindHandler(type.name);
        if (handler == nullptr)
            throw NotRegistered(Type(type.name));

        if (Contains(Handle(id, Owner(), type, HandleObjectType::Shard)))
            throw CannotCreate(type);

        handler->Create(id, Owner(), type.isConst);
    }

    void ReliquaryShards::Destroy(const Type& type, RelicID id)
    {
        const auto handler = FindHandler(type.name);
        if (handler == nullptr)
            throw NotRegistered(type);

        handler->Destroy(id, Owner());
    }

    void ReliquaryShards::Clear()
    {
        for (auto& loop : handlers)
            loop->Clear();
    }

    void ReliquaryShards::Clear(RelicID id)
    {
        auto matrixSnapshot = Owner().matrices.DestroyingSnapshot(id);

        for (auto& handler : Shards().handlers)
        {
            if (handler->BatchSource().ContainsFromBase(id))
            {
                const Type type{ handler->typeName, false };
                Owner().Raise<Destroying>(HandleFrom(id, type, HandleObjectType::Shard));
                matrixSnapshot.Finalize(type);
            }

            if (handler->ConstBatchSource().ContainsFromBase(id))
            {
                const Type type{ handler->typeName, true };
                Owner().Raise<Destroying>(HandleFrom(id, type, HandleObjectType::Shard));
                matrixSnapshot.Finalize(type);
            }
        }

        for (auto& handler : Shards().handlers)
        {
            if (handler->BatchSource().ContainsFromBase(id))
                handler->BatchSource().DestroyFromBase(id);

            if (handler->ConstBatchSource().ContainsFromBase(id))
                handler->ConstBatchSource().DestroyFromBase(id);
        }
    }

    bool ReliquaryShards::Contains(const Handle& handle) const
    {
        assert(handle.ObjectType() == HandleObjectType::Shard);

        const auto id = handle.ID();

        const auto shardBatchSource = FindBatchSource(handle.Type());
        if (shardBatchSource != nullptr)
            return shardBatchSource->ContainsFromBase(id);

        return false;
    }

    ReliquaryShards::HandlerBase::~HandlerBase() = default;

    TypeName ReliquaryShards::HandlerBase::MainType() const
    {
        return typeName;
    }

    ReliquaryShards::HandlerBase::HandlerBase(const TypeName& typeName) : typeName(typeName)
    {}

    ReliquaryShards::HandlerBase* ReliquaryShards::FindHandler(const TypeName& typeName) const
    {
        const auto found = std::find_if(
            handlers.begin(),
            handlers.end(),
            [typeName](const HandlerPtr& entry)
            {
                return entry->typeName == typeName;
            });
        if (found == handlers.end())
            return nullptr;

        return found->get();
    }

    ShardBatchSourceBase* ReliquaryShards::FindBatchSource(const Type& type) const
    {
        const auto found = FindHandler(type.name);
        if (found == nullptr)
            return nullptr;

        return !type.isConst
            ? &found->BatchSource()
            : &found->ConstBatchSource();
    }

    ShardBatchSourceBase& ReliquaryShards::RequiredBatchSource(const Type& type) const
    {
        const auto found = FindBatchSource(type);
        if (!found)
            throw NotRegistered(Type(type));

        return *found;
    }

    ReliquaryShards::ReliquaryShards(Reliquary& owner) : ReliquaryComponent(owner, "shard")
    {}
}
