#include "ReliquaryRelics.h"

#include <cassert>

#include "Reliquary.h"

#include "Destroying.h"
#include "RelicParented.h"

namespace Arca
{
    void ReliquaryRelics::Destroy(const TypeName& typeName, RelicID id)
    {
        const auto handler = FindLocalHandler(typeName);
        if (handler == nullptr)
            throw NotRegistered(Type(typeName));

        handler->Destroy(id, Owner());
    }

    void ReliquaryRelics::Clear()
    {
        for (auto& loop : localHandlers)
            loop->Clear();

        for (auto loop = metadataList.begin(); loop != metadataList.end();)
        {
            if (loop->locality == Arca::Locality::Global && !loop->shouldSerialize)
                ++loop;
            else
                loop = metadataList.erase(loop);
        }

        nextRelicID = 1;
    }

    void ReliquaryRelics::Clear(const TypeName& typeName)
    {
        auto& batchSource = RequiredBatchSource(typeName);
        batchSource.DestroyAllFromBase(Owner());
    }

    std::optional<Handle> ReliquaryRelics::ParentOf(const Handle& child) const
    {
        const auto childID = child.ID();
        const auto metadata = MetadataFor(childID);
        if (!metadata)
            throw CannotFind(child.Type());

        if (!metadata->parent)
            return {};

        return Handle(
            metadata->parent->ID(),
            const_cast<Reliquary&>(Owner()),
            metadata->parent->Type(),
            HandleObjectType::Relic);
    }

    RelicMetadata* ReliquaryRelics::SetupNewInternals(
        RelicID id,
        Openness openness,
        Locality locality,
        bool shouldSerialize,
        Type type)
    {
        metadataList.emplace_back(id, openness, locality, std::move(type), nullptr, shouldSerialize);
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
            Shards().Create(entry, id);
    }

    bool ReliquaryRelics::WillDestroy(RelicMetadata* metadata) const
    {
        return metadata && metadata->locality != Locality::Global;
    }

    void ReliquaryRelics::Destroy(RelicMetadata& metadata)
    {
        Owner().Raise<Destroying>(HandleFrom(metadata));

        for (auto& child : metadata.children)
            Destroy(*MetadataFor(child.ID()));

        const auto id = metadata.id;

        auto matrixSnapshot = Owner().matrices.DestroyingSnapshot(metadata.id);

        const auto attemptDestroyFromBase = [metadata, &matrixSnapshot, id, this](ShardBatchSourceBase& batchSource, bool isConst, ReliquaryShards::HandlerBase& handler)
        {
            if (batchSource.ContainsFromBase(id))
            {
                const Type type{ handler.typeName, isConst };
                Owner().Raise<Destroying>(HandleFrom(id, type, HandleObjectType::Shard));
                matrixSnapshot.Finalize(type);
                batchSource.DestroyFromBase(id);
            }
        };

        for(auto& handler : Shards().handlers)
        {
            attemptDestroyFromBase(handler->BatchSource(), false, *handler);
            attemptDestroyFromBase(handler->ConstBatchSource(), true, *handler);
        }

        if (metadata.parent)
        {
            const auto parent = *metadata.parent;
            const auto parentMetadata = MetadataFor(parent.ID());
            const auto eraseChildrenItr =
                std::remove_if(
                    parentMetadata->children.begin(),
                    parentMetadata->children.end(),
                    [id](const HandleSlim& child)
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

    bool ReliquaryRelics::CanModifyShards(RelicID id) const
    {
        const auto metadata = MetadataFor(id);
        return metadata && metadata->openness == Openness::Open;
    }

    void ReliquaryRelics::ShardModificationRequired(RelicID id) const
    {
        if (!CanModifyShards(id))
            throw CannotModify(id);
    }

    void ReliquaryRelics::ThrowIfCannotParent(const Handle& parent, RelicPrototype child)
    {
        ValidateParentForParenting(parent);

        assert(parent.ID() != child.id);
    }

    void ReliquaryRelics::Parent(const Handle& parent, const Handle& child)
    {
        auto& parentMetadata = ValidateParentForParenting(parent);

        assert(&child.Owner() == &Owner());

        const auto childMetadata = MetadataFor(child.ID());
        assert(childMetadata != nullptr);
        assert(childMetadata->locality != Locality::Global);
        assert(!childMetadata->parent.has_value());

        parentMetadata.children.push_back(child);
        childMetadata->parent = parent;

        Owner().Raise<RelicParented>({ parent, child });
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
            throw NotRegistered(Type(typeName));

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
        if (&parent.Owner() != &Owner())
            throw CannotParentRelic(
                "The parent relic is from a different Reliquary.");

        const auto parentMetadata = MetadataFor(parent.ID());
        if (!parentMetadata)
            throw CannotFind(parent.Type());

        if (parentMetadata->locality == Locality::Global)
            throw CannotParentRelic(
                "Attempted to parent to a global relic.");

        return *parentMetadata;
    }

    ReliquaryRelics::ReliquaryRelics(Reliquary& owner) : ReliquaryComponent(owner, "relic")
    {}
}
