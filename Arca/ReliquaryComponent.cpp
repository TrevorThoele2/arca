#include "ReliquaryComponent.h"

#include "Reliquary.h"

namespace Arca
{
    NotRegistered ReliquaryComponent::NotRegistered(
        const TypeHandle& type) const
    {
        return Arca::NotRegistered(objectTypeName, type);
    }

    NotRegistered ReliquaryComponent::NotRegistered(
        const TypeHandle& type,
        const std::type_index& classType) const
    {
        return Arca::NotRegistered(objectTypeName, type, classType);
    }

    CannotModify ReliquaryComponent::CannotModify(RelicID id) const
    {
        return Arca::CannotModify(objectTypeName, id);
    }

    ReliquaryComponent::ReliquaryComponent(Reliquary& owner, std::string objectTypeName)
        : owner(&owner), objectTypeName(std::move(objectTypeName))
    {}

    Reliquary& ReliquaryComponent::Owner()
    {
        return *owner;
    }

    const Reliquary& ReliquaryComponent::Owner() const
    {
        return *owner;
    }

    ReliquaryRelics& ReliquaryComponent::Relics()
    {
        return owner->relics;
    }

    const ReliquaryRelics& ReliquaryComponent::Relics() const
    {
        return owner->relics;
    }

    ReliquaryShards& ReliquaryComponent::Shards()
    {
        return owner->shards;
    }

    const ReliquaryShards& ReliquaryComponent::Shards() const
    {
        return owner->shards;
    }

    ReliquaryCurators& ReliquaryComponent::Curators()
    {
        return owner->curators;
    }

    const ReliquaryCurators& ReliquaryComponent::Curators() const
    {
        return owner->curators;
    }

    ReliquarySignals& ReliquaryComponent::Signals()
    {
        return owner->signals;
    }

    const ReliquarySignals& ReliquaryComponent::Signals() const
    {
        return owner->signals;
    }

    Handle ReliquaryComponent::HandleFrom(RelicID id)
    {
        return Handle{ id, Owner() };
    }

    Handle ReliquaryComponent::HandleFrom(const RelicMetadata& metadata)
    {
        return HandleFrom(metadata.id);
    }
}