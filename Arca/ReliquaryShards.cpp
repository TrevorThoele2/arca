#include "ReliquaryShards.h"

#include "Reliquary.h"
#include "ReliquaryRelics.h"
#include "ReliquaryException.h"

namespace Arca
{
    void ReliquaryShards::Create(const Type& type, RelicID id, bool required)
    {
        const auto handler = FindHandler(type.name);
        if (handler == nullptr)
            throw NotRegistered(objectTypeName, Type(type.name));

        if (Contains(Handle{ id, type }))
            throw CannotCreate(objectTypeName, type);

        handler->Create(id, *matrices, *owner, type.isConst, required);
    }
    
    void ReliquaryShards::TransactionalDestroy(const Type& type, RelicID id)
    {
        const auto handler = FindHandler(type.name);
        if (handler == nullptr)
            throw NotRegistered(objectTypeName, type);

        handler->RequiredTransactionalDestroy(id, *owner, *signals, *matrices);
    }

    void ReliquaryShards::Clear()
    {
        for (auto& loop : handlers)
            loop->Clear();
    }

    void ReliquaryShards::Clear(RelicID id)
    {
        for (auto& handler : handlers)
        {
            if (handler->BatchSource().ContainsFromBase(id))
            {
                const Type type{ handler->type.name, false };
                matrices->ShardDestroying(id, type, AllTypes(id));
            }

            if (handler->ConstBatchSource().ContainsFromBase(id))
            {
                const Type type{ handler->type.name, true };
                matrices->ShardDestroying(id, type, AllTypes(id));
            }
        }

        for (auto& handler : handlers)
            handler->RequiredDestroy(id, *owner, *signals);
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

    std::unordered_set<Type> ReliquaryShards::AllTypes(RelicID id) const
    {
        const auto found = idToTypes.find(id);
        return found == idToTypes.end() ? std::unordered_set<Type>{} : found->second;
    }

    size_t ReliquaryShards::Size(const Type& type) const
    {
        auto handler = FindHandler(type.name);
        return handler
            ? !type.isConst ? handler->BatchSource().Size() : handler->ConstBatchSource().Size()
            : 0;
    }

    void ReliquaryShards::AddType(RelicID id, const Type& type)
    {
        auto found = idToTypes.find(id);
        if (found == idToTypes.end())
            idToTypes.emplace(id, std::unordered_set<Type>{ type });
        else
            found->second.insert(type);
    }

    void ReliquaryShards::RemoveType(RelicID id, const Type& type)
    {
        auto found = idToTypes.find(id);
        if (found != idToTypes.end())
        {
            found->second.erase(type);
            if (found->second.empty())
                idToTypes.erase(found);
        }
    }

    bool ReliquaryShards::Contains(const Handle& handle) const
    {
        const auto id = handle.id;

        const auto shardBatchSource = FindBatchSource(handle.type);
        return shardBatchSource != nullptr ? shardBatchSource->ContainsFromBase(id) : false;
    }

    ReliquaryShards::HandlerBase::~HandlerBase() = default;

    TypeName ReliquaryShards::HandlerBase::MainType() const
    {
        return type.name;
    }

    ReliquaryShards::HandlerBase::HandlerBase(const Type& type) : type(type)
    {}

    ReliquaryShards::HandlerBase* ReliquaryShards::FindHandler(const TypeName& typeName) const
    {
        const auto found = std::find_if(
            handlers.begin(),
            handlers.end(),
            [typeName](const HandlerPtr& entry)
            {
                return entry->type.name == typeName;
            });
        return found == handlers.end() ? nullptr : found->get();
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

    ReliquaryShards::ReliquaryShards(Reliquary& owner, ReliquarySignals& signals, ReliquaryMatrices& matrices) :
        owner(&owner), signals(&signals), matrices(&matrices)
    {}
}
