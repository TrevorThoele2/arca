#include "ReliquaryRelics.h"

#include <cassert>

#include "Reliquary.h"

#include "Destroying.h"
#include "RelicParented.h"

namespace Arca
{
    void ReliquaryRelics::SetupNewInternals(
        RelicID id,
        RelicOpenness openness,
        std::optional<TypeHandle> typeHandle,
        void* storage)
    {
        metadataList.emplace_back(id, openness, std::move(typeHandle), storage);
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
        return metadata && metadata->openness != RelicOpenness::Global;
    }

    void ReliquaryRelics::Destroy(RelicMetadata& metadata)
    {
        Owner().Raise<Destroying>(HandleFrom(metadata));

        for (auto& child : metadata.children)
            Destroy(*MetadataFor(child));

        auto& id = metadata.id;

        for (auto& shardBatchSource : Shards().batchSources.map)
        {
            if (shardBatchSource.second->DestroyFromBase(id))
                Owner().Raise<Destroying>(HandleFrom(id));
        }

        if (metadata.parent)
        {
            const auto parent = *metadata.parent;
            const auto parentMetadata = MetadataFor(parent);
            const auto eraseChildrenItr =
                std::remove_if(
                    parentMetadata->children.begin(),
                    parentMetadata->children.end(),
                    [id](const RelicID& childId)
                    {
                        return id == childId;
                    });
            if (eraseChildrenItr != parentMetadata->children.end())
                parentMetadata->children.erase(eraseChildrenItr);
        }

        if (metadata.typeHandle)
        {
            auto batchSource = batchSources.Find(metadata.typeHandle->name);
            batchSource->DestroyFromBase(id);
        }

        DestroyMetadata(id);
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

    bool ReliquaryRelics::CanModify(RelicID id) const
    {
        const auto metadata = MetadataFor(id);
        return metadata && metadata->openness == RelicOpenness::Open;
    }

    void ReliquaryRelics::ModificationRequired(RelicID id) const
    {
        if (!CanModify(id))
            throw CannotModify(id);
    }

    void ReliquaryRelics::ThrowIfCannotParent(const Handle& parent, RelicPrototype child)
    {
        ValidateParentForParenting(parent);

        assert(parent.ID() != child.id);
        assert(child.openness != RelicOpenness::Global);
    }

    void ReliquaryRelics::Parent(const Handle& parent, const Handle& child)
    {
        auto& parentMetadata = ValidateParentForParenting(parent);

        assert(&child.Owner() == &Owner());

        const auto childMetadata = MetadataFor(child.ID());
        assert(childMetadata != nullptr);
        assert(childMetadata->openness != RelicOpenness::Global);
        assert(!childMetadata->parent.has_value());

        parentMetadata.children.push_back(child.ID());
        childMetadata->parent = parent.ID();

        Owner().Raise<RelicParented>({ parent, child });
    }

    ReliquaryRelics::BatchSources::BatchSources(ReliquaryRelics& owner) : BatchSourcesBase(owner)
    {}

    RelicMetadata& ReliquaryRelics::ValidateParentForParenting(const Handle& parent)
    {
        if (&parent.Owner() != &Owner())
            throw CannotParentRelic(
                "The parent relic is from a different Reliquary.");

        const auto parentMetadata = MetadataFor(parent.ID());
        if (!parentMetadata)
            throw CannotFindRelic(parent.ID());

        if (parentMetadata->openness == RelicOpenness::Global)
            throw CannotParentRelic(
                "Attempted to parent to a global relic.");

        return *parentMetadata;
    }

    ReliquaryRelics::ReliquaryRelics(Reliquary& owner) : ReliquaryComponent(owner, "relic")
    {}
}
