#include "ReliquaryRelics.h"

#include <cassert>

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

        handler->Destroy(id, *owner);
    }

    void ReliquaryRelics::Clear()
    {
        for (auto& loop : localHandlers)
            loop->Clear(*owner);

        for (auto loop = metadataList.begin(); loop != metadataList.end();)
        {
            if (loop->locality == Arca::Locality::Global)
                ++loop;
            else
                loop = metadataList.erase(loop);
        }

        nextRelicID = 1;
    }

    void ReliquaryRelics::Clear(const TypeName& typeName)
    {
        auto& batchSource = RequiredBatchSource(typeName);
        batchSource.DestroyAllFromBase(*owner);
    }

    bool ReliquaryRelics::Contains(RelicID id) const
    {
        const auto metadata = MetadataFor(id);
        return metadata;
    }

    std::optional<Handle> ReliquaryRelics::ParentOf(RelicID childID) const
    {
        const auto metadata = MetadataFor(childID);
        if (!metadata)
            return {};

        if (!metadata->parent)
            return {};

        return Handle(
            metadata->parent->ID(),
            const_cast<Reliquary&>(*owner),
            metadata->parent->Type(),
            HandleObjectType::Relic);
    }

    std::vector<Handle> ReliquaryRelics::ChildrenOf(RelicID parentID) const
    {
        std::vector<Handle> returnValue;

        for(auto& metadata : metadataList)
        {
            if (!metadata.parent)
                continue;

            if (metadata.parent->ID() != parentID)
                continue;

            returnValue.emplace_back(
                metadata.id,
                const_cast<Reliquary&>(*owner),
                metadata.type,
                HandleObjectType::Relic);
        }

        return returnValue;
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
        metadataList.emplace_back(
            id,
            locality,
            std::move(type),
            nullptr,
            shouldSerializeBinary,
            shouldSerializeJson);
        return &metadataList.back();
    }

    void ReliquaryRelics::DestroyMetadata(RelicID id)
    {
        const auto itr = std::remove_if(
            metadataList.begin(),
            metadataList.end(),
            [id](const RelicMetadata& metadata)
            {
                return metadata.id == id;
            });
        if (itr == metadataList.end())
            return;
        metadataList.erase(itr);
    }

    auto ReliquaryRelics::MetadataFor(RelicID id) -> RelicMetadata*
    {
        const auto found = std::find_if(
            metadataList.begin(),
            metadataList.end(),
            [id](const RelicMetadata& metadata)
            {
                return metadata.id == id;
            });
        return found != metadataList.end()
            ? &*found
            : nullptr;
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
        owner->Raise(Destroying{ Handle{ metadata.id, const_cast<Reliquary&>(*owner), metadata.type, HandleObjectType::Relic } });

        while(!metadata.children.empty())
            Destroy(*MetadataFor(metadata.children[0].ID()));

        const auto id = metadata.id;

        shards->Clear(id);

        if (metadata.parent)
        {
            const auto parent = *metadata.parent;
            const auto parentMetadata = MetadataFor(parent.ID());
            const auto eraseChildrenItr =
                std::remove_if(
                    parentMetadata->children.begin(),
                    parentMetadata->children.end(),
                    [id](const SlimHandle& child)
                    {
                        return id == child.ID();
                    });
            if (eraseChildrenItr != parentMetadata->children.end())
                parentMetadata->children.erase(eraseChildrenItr);
        }

        auto batchSource = FindBatchSource(metadata.type.name);
        batchSource->DestroyFromBase(id);

        DestroyMetadata(id);
    }

    std::vector<RelicID> ReliquaryRelics::AllIDs() const
    {
        std::vector<RelicID> returnValue;
        for (auto& loop : metadataList)
            returnValue.push_back(loop.id);
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

    void ReliquaryRelics::ThrowIfCannotParent(const Handle& parent, RelicPrototype child)
    {
        ValidateParentForParenting(parent);

        assert(parent.ID() != child.id);
    }

    void ReliquaryRelics::Parent(const Handle& parent, const Handle& child)
    {
        auto& parentMetadata = ValidateParentForParenting(parent);

        assert(child.Owner() == owner);

        const auto childMetadata = MetadataFor(child.ID());
        assert(childMetadata != nullptr);
        assert(childMetadata->locality != Locality::Global);
        assert(!childMetadata->parent.has_value());

        parentMetadata.children.push_back(child);
        childMetadata->parent = parent;
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
        if (found == localHandlers.end())
            return nullptr;

        return found->get();
    }

    RelicBatchSourceBase* ReliquaryRelics::FindBatchSource(const TypeName& typeName) const
    {
        const auto found = FindLocalHandler(typeName);
        if (found == nullptr)
            return nullptr;

        return &found->BatchSource();
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
        if (found == globalHandlers.end())
            return nullptr;

        return found->get();
    }

    RelicMetadata& ReliquaryRelics::ValidateParentForParenting(const Handle& parent)
    {
        if (parent.Owner() != owner)
            throw CannotParentRelic(
                "Cannot parent a relic to a relic in a different Reliquary.");

        const auto parentMetadata = MetadataFor(parent.ID());
        if (!parentMetadata)
            throw CannotFind(objectTypeName, parent.Type());

        if (parentMetadata->locality == Locality::Global)
            throw CannotParentRelic(
                "Cannot parent a relic to a global relic.");

        return *parentMetadata;
    }

    ReliquaryRelics::ReliquaryRelics(
        Reliquary& owner, ReliquaryRelicStructures& relicStructures, ReliquaryShards& shards)
        :
        owner(&owner), relicStructures(&relicStructures), shards(&shards)
    {}
}
