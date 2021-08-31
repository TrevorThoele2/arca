#include "ReliquaryShards.h"

#include "Reliquary.h"
#include "ReliquaryRelics.h"
#include "ReliquaryException.h"
#include <cassert>

namespace Arca
{
    void ReliquaryShards::Create(const Type& type, RelicID id, bool required)
    {
        const auto handler = FindHandler(type.name);
        if (handler == nullptr)
            throw NotRegistered(objectTypeName, Type(type.name));

        if (Contains(Handle(id, *owner, type, HandleObjectType::Shard)))
            throw CannotCreate(objectTypeName, type);

        handler->Create(id, *owner, type.isConst, required);
    }

    void ReliquaryShards::TransactionalDestroy(const Type& type, RelicID id)
    {
        const auto handler = FindHandler(type.name);
        if (handler == nullptr)
            throw NotRegistered(objectTypeName, type);

        handler->RequiredTransactionalDestroy(id, *owner, owner->matrices);
    }

    void ReliquaryShards::Clear()
    {
        for (auto& loop : handlers)
            loop->Clear();
    }

    void ReliquaryShards::Clear(RelicID id)
    {
        auto matrixSnapshot = owner->matrices.StartDestroyingTransaction(id);

        for (auto& handler : handlers)
        {
            if (handler->BatchSource().ContainsFromBase(id))
            {
                const Type type{ handler->typeName, false };
                matrixSnapshot.Finalize(type);
            }

            if (handler->ConstBatchSource().ContainsFromBase(id))
            {
                const Type type{ handler->typeName, true };
                matrixSnapshot.Finalize(type);
            }
        }

        for (auto& handler : handlers)
            handler->RequiredDestroy(id, *owner);
    }

    bool ReliquaryShards::IsShardTypeName(const TypeName& typeName) const
    {
        for (auto& checkTypeName : AllTypeNames())
            if (checkTypeName == typeName)
                return true;

        return false;
    }

    std::vector<TypeName> ReliquaryShards::AllTypeNames() const
    {
        std::vector<TypeName> returnValue;
        for (auto& handler : handlers)
            returnValue.push_back(handler->MainType());
        return returnValue;
    }

    size_t ReliquaryShards::Size(const Type& type) const
    {
        auto handler = FindHandler(type.name);
        return handler
            ? !type.isConst ? handler->BatchSource().Size() : handler->ConstBatchSource().Size()
            : 0;
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
            throw NotRegistered(objectTypeName, Type(type));

        return *found;
    }

    ReliquaryShards::ReliquaryShards(Reliquary& owner) : owner(&owner)
    {}
}
