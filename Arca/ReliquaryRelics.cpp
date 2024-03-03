#include "ReliquaryRelics.h"

#include "Reliquary.h"

#include "Destroying.h"

namespace Arca
{
    void ReliquaryRelics::SetupNewInternals(
        RelicID id,
        RelicDynamism dynamism,
        std::optional<TypeHandle> typeHandle,
        void* storage)
    {
        metadataList.emplace_back(id, dynamism, std::move(typeHandle), storage);
        occupiedIDs.Include(id);
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
        occupiedIDs.Remove(id);
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
        return metadata && metadata->dynamism != RelicDynamism::Static;
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
        const auto itr = occupiedIDs.begin();
        return itr == occupiedIDs.end() || itr->Start() > 1
            ? 1
            : (--occupiedIDs.end())->End() + 1;
    }

    bool ReliquaryRelics::CanModify(RelicID id) const
    {
        const auto metadata = MetadataFor(id);
        return metadata && metadata->dynamism != RelicDynamism::Fixed;
    }

    void ReliquaryRelics::ModificationRequired(RelicID id) const
    {
        if (!CanModify(id))
            throw CannotModify(id);
    }

    ReliquaryRelics::BatchSources::BatchSources(ReliquaryRelics& owner) : BatchSourcesBase(owner)
    {}

    ReliquaryRelics::ReliquaryRelics(Reliquary& owner) : ReliquaryComponent(owner, "relic")
    {}
}
