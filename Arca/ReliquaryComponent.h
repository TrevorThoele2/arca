#pragma once

#include <vector>

#include "KnownPolymorphicSerializer.h"

#include "RelicMetadata.h"
#include "Handle.h"
#include "Ptr.h"
#include "ReliquaryException.h"

namespace Arca
{
    struct RelicMetadata;
    class Reliquary;
    class ReliquaryRelics;
    class ReliquaryShards;
    class ReliquaryCurators;
    class ReliquarySignals;

    class ReliquaryComponent
    {
    public:
        using KnownPolymorphicSerializerList = std::vector<KnownPolymorphicSerializer>;
    public:
        [[nodiscard]] Arca::NotRegistered NotRegistered(
            const TypeHandle& type) const;
        [[nodiscard]] Arca::NotRegistered NotRegistered(
            const TypeHandle& type,
            const std::type_index& classType) const;
        [[nodiscard]] CannotModify CannotModify(RelicID id) const;
    protected:
        explicit ReliquaryComponent(Reliquary& owner, std::string objectTypeName);
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
        [[nodiscard]] ReliquaryRelics& Relics();
        [[nodiscard]] const ReliquaryRelics& Relics() const;
        [[nodiscard]] ReliquaryShards& Shards();
        [[nodiscard]] const ReliquaryShards& Shards() const;
        [[nodiscard]] ReliquaryCurators& Curators();
        [[nodiscard]] const ReliquaryCurators& Curators() const;
        [[nodiscard]] ReliquarySignals& Signals();
        [[nodiscard]] const ReliquarySignals& Signals() const;
    protected:
        Handle HandleFrom(RelicID id);
        Handle HandleFrom(const RelicMetadata& metadata);
        template<class T>
        Ptr<T> PtrFrom(RelicID id);
        template<class T>
        Ptr<T> PtrFrom(const RelicMetadata& metadata);
    private:
        Reliquary* owner;
    private:
        const std::string objectTypeName;
    private:
        friend Reliquary;
    };

    template<class T>
    Ptr<T> ReliquaryComponent::PtrFrom(RelicID id)
    {
        return Ptr<T>(id, Owner());
    }

    template<class T>
    Ptr<T> ReliquaryComponent::PtrFrom(const RelicMetadata& metadata)
    {
        return Ptr<T>(metadata.id, Owner());
    }
}
