#include "ReliquaryComponent.h"
#include <utility>

#include "Reliquary.h"

namespace Arca
{
    NotRegistered ReliquaryComponent::NotRegistered(
        const Type& type) const
    {
        return CreateException<Arca::NotRegistered>(type);
    }

    NotRegistered ReliquaryComponent::NotRegistered(
        const Type& type,
        const std::type_index& classType) const
    {
        return CreateException<Arca::NotRegistered>(type, classType);
    }

    Arca::CannotCreate ReliquaryComponent::CannotCreate(
        const Type& type) const
    {
        return CreateException<Arca::CannotCreate>(type);
    }

    Arca::CannotCreate ReliquaryComponent::CannotCreate(
        const Type& type,
        const std::type_index& classType) const
    {
        return CreateException<Arca::CannotCreate>(type, classType);
    }

    Arca::CannotDestroy ReliquaryComponent::CannotDestroy(
        const Type& type) const
    {
        return CreateException<Arca::CannotDestroy>(type);
    }

    Arca::CannotDestroy ReliquaryComponent::CannotDestroy(
        const Type& type,
        const std::type_index& classType) const
    {
        return CreateException<Arca::CannotDestroy>(type, classType);
    }

    Arca::CannotFind ReliquaryComponent::CannotFind(
        const Type& type) const
    {
        return CreateException<Arca::CannotFind>(type);
    }

    Arca::CannotFind ReliquaryComponent::CannotFind(
        const Type& type,
        const std::type_index& classType) const
    {
        return CreateException<Arca::CannotFind>(type, classType);
    }

    CannotModifyShards ReliquaryComponent::CannotModify(RelicID id) const
    {
        return Arca::CannotModifyShards(objectTypeName, id);
    }

    Reliquary& ReliquaryComponent::Owner()
    {
        return *owner;
    }

    const Reliquary& ReliquaryComponent::Owner() const
    {
        return *owner;
    }

    ReliquaryComponent::ReliquaryComponent(Reliquary& owner, std::string objectTypeName)
        : owner(&owner), objectTypeName(std::move(objectTypeName))
    {}

    ReliquaryRelics& ReliquaryComponent::Relics()
    {
        return owner->relics;
    }

    const ReliquaryRelics& ReliquaryComponent::Relics() const
    {
        return owner->relics;
    }

    ReliquaryRelicStructures& ReliquaryComponent::RelicStructures()
    {
        return owner->relicStructures;
    }

    const ReliquaryRelicStructures& ReliquaryComponent::RelicStructures() const
    {
        return owner->relicStructures;
    }

    ReliquaryShards& ReliquaryComponent::Shards()
    {
        return owner->shards;
    }

    const ReliquaryShards& ReliquaryComponent::Shards() const
    {
        return owner->shards;
    }

    ReliquaryMatrices& ReliquaryComponent::Matrices()
    {
        return owner->matrices;
    }

    const ReliquaryMatrices& ReliquaryComponent::Matrices() const
    {
        return owner->matrices;
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

    ReliquaryCommands& ReliquaryComponent::Commands()
    {
        return owner->commands;
    }

    const ReliquaryCommands& ReliquaryComponent::Commands() const
    {
        return owner->commands;
    }

    Handle ReliquaryComponent::HandleFrom(RelicID id, Type type, HandleObjectType objectType) const
    {
        return Handle{ id, const_cast<Reliquary&>(Owner()), std::move(type), objectType };
    }

    Handle ReliquaryComponent::HandleFrom(const RelicMetadata& metadata) const
    {
        return HandleFrom(metadata.id, metadata.type, HandleObjectType::Relic);
    }
}