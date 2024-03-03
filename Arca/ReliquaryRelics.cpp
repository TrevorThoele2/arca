#include "ReliquaryRelics.h"

#include "Reliquary.h"
#include "ReliquaryException.h"

#include "Destroying.h"

namespace Arca
{
    void ReliquaryRelics::Destroy(const TypeName& typeName, RelicID id)
    {
        const auto handler = FindLocalHandler(typeName);
        if (handler == nullptr)
            throw NotRegistered(objectTypeName, Type(typeName));

        handler->Destroy(id, *this);
    }

    void ReliquaryRelics::Clear()
    {
        for (auto& loop : localHandlers)
            loop->Clear(*this);

        for (auto loop = metadata.begin(); loop != metadata.end();)
        {
            if (loop->second.locality == Locality::Global)
                ++loop;
            else
                loop = metadata.erase(loop);
        }

        nextRelicID = 1;
    }

    void ReliquaryRelics::Clear(const TypeName& typeName)
    {
        auto localHandler = FindLocalHandler(typeName);
        if (localHandler)
            localHandler->Clear(*this);
        else
            throw NotRegistered(objectTypeName, Type{ typeName, false });
    }

    bool ReliquaryRelics::Contains(RelicID id) const
    {
        const auto metadata = MetadataFor(id);
        return metadata;
    }

    bool ReliquaryRelics::IsRelicTypeName(const TypeName& typeName) const
    {
        for (auto& checkTypeName : AllTypeNames())
            if (checkTypeName == typeName)
                return true;

        return false;
    }

    std::vector<TypeName> ReliquaryRelics::AllTypeNames() const
    {
        std::vector<TypeName> returnValue;
        for (auto& handler : localHandlers)
            returnValue.push_back(handler->MainType());
        for (auto& handler : globalHandlers)
            returnValue.push_back(handler->MainType());
        return returnValue;
    }

    size_t ReliquaryRelics::Size(const TypeName& typeName) const
    {
        auto handler = FindLocalHandler(typeName);
        return handler ? handler->BatchSource().Size() : 0;
    }

    RelicMetadata* ReliquaryRelics::SetupNewMetadata(
        RelicID id,
        Locality locality,
        bool shouldSerializeBinary,
        bool shouldSerializeJson,
        Type type)
    {
        return &metadata.emplace(
            id,
            RelicMetadata
            {
                id,
                locality,
                std::move(type),
                nullptr,
                shouldSerializeBinary,
                shouldSerializeJson
            }).first->second;
    }

    void ReliquaryRelics::DestroyMetadata(RelicID id)
    {
        metadata.erase(id);
    }

    auto ReliquaryRelics::MetadataFor(RelicID id) -> RelicMetadata*
    {
        const auto found = metadata.find(id);
        return found != metadata.end() ? &found->second : nullptr;
    }

    auto ReliquaryRelics::MetadataFor(RelicID id) const -> const RelicMetadata*
    {
        return const_cast<ReliquaryRelics*>(this)->MetadataFor(id);
    }

    void ReliquaryRelics::SatisfyStructure(RelicID id, const RelicStructure& structure)
    {
        for (auto& entry : structure)
            shards->Create(entry, id, true);
    }

    bool ReliquaryRelics::WillDestroy(RelicMetadata* metadata) const
    {
        return metadata && metadata->locality != Locality::Global;
    }

    void ReliquaryRelics::Destroy(RelicMetadata& metadata)
    {
        owner->Raise(Destroying{ Handle{ metadata.id, metadata.type }});

        const auto id = metadata.id;

        shards->Clear(id);

        auto batchSource = FindBatchSource(metadata.type.name);
        batchSource->DestroyFromBase(id);

        DestroyMetadata(id);
    }

    std::vector<RelicID> ReliquaryRelics::AllIDs() const
    {
        std::vector<RelicID> returnValue;
        for (auto& [id, _] : metadata)
            returnValue.push_back(id);
        return returnValue;
    }

    RelicID ReliquaryRelics::NextID() const
    {
        return nextRelicID;
    }

    RelicID ReliquaryRelics::AdvanceID()
    {
        const auto returnValue = nextRelicID;
        ++nextRelicID;
        return returnValue;
    }
    
    ReliquaryRelics::LocalHandlerBase::~LocalHandlerBase() = default;

    TypeName ReliquaryRelics::LocalHandlerBase::MainType() const
    {
        return typeName;
    }

    ReliquaryRelics::LocalHandlerBase::LocalHandlerBase(const TypeName& typeName) : typeName(typeName)
    {}

    ReliquaryRelics::LocalHandlerBase* ReliquaryRelics::FindLocalHandler(const TypeName& typeName) const
    {
        const auto found = std::find_if(
            localHandlers.begin(),
            localHandlers.end(),
            [typeName](const LocalHandlerPtr& entry)
            {
                return entry->typeName == typeName;
            });
        return found == localHandlers.end() ? nullptr : found->get();
    }

    RelicBatchSourceBase* ReliquaryRelics::FindBatchSource(const TypeName& typeName) const
    {
        const auto found = FindLocalHandler(typeName);
        return found == nullptr ? nullptr : &found->BatchSource();
    }

    RelicBatchSourceBase& ReliquaryRelics::RequiredBatchSource(const TypeName& typeName) const
    {
        const auto found = FindBatchSource(typeName);
        if (!found)
            throw NotRegistered(objectTypeName, Type(typeName));

        return *found;
    }

    ReliquaryRelics::GlobalHandlerBase::~GlobalHandlerBase() = default;

    TypeName ReliquaryRelics::GlobalHandlerBase::MainType() const
    {
        return typeName;
    }

    ReliquaryRelics::GlobalHandlerBase::GlobalHandlerBase(
        const TypeName& typeName, RelicID id)
        :
        typeName(typeName), id(id)
    {}

    ReliquaryRelics::GlobalHandlerBase* ReliquaryRelics::FindGlobalHandler(const TypeName& typeName) const
    {
        const auto found = std::find_if(
            globalHandlers.begin(),
            globalHandlers.end(),
            [typeName](const GlobalHandlerPtr& entry)
            {
                return entry->typeName == typeName;
            });
        return found == globalHandlers.end() ? nullptr : found->get();
    }

    ReliquaryRelics::ReliquaryRelics(
        Reliquary& owner,
        ReliquaryRelicStructures& relicStructures,
        ReliquaryShards& shards,
        ReliquarySignals& signals)
        :
        owner(&owner), relicStructures(&relicStructures), shards(&shards), signals(&signals)
    {}
}
