#pragma once

#include "ReliquaryRelics.h"
#include "Created.h"

namespace Arca
{
    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::Create(CreationArgs&& ... creationArgs)
    {
        auto relic = CreateRelic<RelicT>(std::forward<CreationArgs>(creationArgs)...);
        if (!relic)
            return {};

        auto pushed = PushNewRelic(std::move(*relic), {});
        return CreatePtr<RelicT>(pushed->ID());
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateWith(const RelicStructure& structure, CreationArgs&& ... creationArgs)
    {
        auto relic = CreateRelic<RelicT>(std::forward<CreationArgs>(creationArgs)...);
        if (!relic)
            return {};

        auto pushed = PushNewRelic(std::move(*relic), structure);
        return CreatePtr<RelicT>(pushed->ID());
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateWith(const std::string& structureName, CreationArgs&& ... creationArgs)
    {
        auto relic = CreateRelic<RelicT>(std::forward<CreationArgs>(creationArgs)...);
        if (!relic)
            return {};

        auto structure = RelicStructures().RequiredRelicStructure(structureName);

        auto pushed = PushNewRelic(std::move(*relic), structure);
        return CreatePtr<RelicT>(pushed->ID());
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateChild(const Handle& parent, CreationArgs&& ... creationArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });
        auto child = Create<RelicT>(std::forward<CreationArgs>(creationArgs)...);
        Parent(parent, AsHandle(*child));
        return child;
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const RelicStructure& structure, CreationArgs&& ... creationArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });
        auto child = CreateWith<RelicT>(structure, std::forward<CreationArgs>(creationArgs)...);
        Parent(parent, AsHandle(*child));
        return child;
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const std::string& structureName, CreationArgs&& ... creationArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });
        auto child = CreateWith<RelicT>(structureName, std::forward<CreationArgs>(creationArgs)...);
        Parent(parent, AsHandle(*child));
        return child;
    }

    template<class RelicT, std::enable_if_t<is_local_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::Find(RelicID id) const
    {
        const auto metadata = MetadataFor(id);
        if (!metadata || metadata->locality != Locality::Local)
            return {};

        auto& batchSource = batchSources.Required<RelicT>();
        auto found = batchSource.Find(id);
        return CreatePtr<RelicT>(id);
    }

    template<class RelicT, std::enable_if_t<is_global_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::Find() const
    {
        const auto type = TypeFor<RelicT>();
        const auto found = globalMap.find(type.name);
        if (found == globalMap.end())
            throw NotRegistered(type, typeid(RelicT));
        
        return CreatePtr<RelicT>(found->second.id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicID ReliquaryRelics::IDFor(const RelicT& relic) const
    {
        return relic.ID();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* ReliquaryRelics::FindStorage(RelicID id)
    {
        const auto metadata = MetadataFor(id);
        if (!metadata)
            return {};

        auto& batchSource = batchSources.Required<RelicT>();
        return batchSource.Find(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    auto ReliquaryRelics::BatchSources::MapFor() -> Map&
    {
        return map;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    auto ReliquaryRelics::BatchSources::MapFor() const -> const Map&
    {
        return map;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* ReliquaryRelics::FindGlobalStorage()
    {
        const auto type = TypeFor<RelicT>();
        const auto found = globalMap.find(type.name);
        if (found == globalMap.end())
            throw NotRegistered(type, typeid(RelicT));

        return reinterpret_cast<RelicT*>(found->second.storage.get());
    }

    template<
        class RelicT,
        class... CreationArgs,
        std::enable_if_t<is_relic_v<RelicT> && has_factory_method_v<RelicT>, int>>
    std::optional<RelicT> ReliquaryRelics::CreateRelic(CreationArgs&& ... creationArgs)
    {
        return Traits<RelicT>::Factory(Owner(), std::forward<CreationArgs>(creationArgs)...);
    }

    template<
        class RelicT,
        class... CreationArgs,
        std::enable_if_t<is_relic_v<RelicT> && !has_factory_method_v<RelicT>, int>>
    std::optional<RelicT> ReliquaryRelics::CreateRelic(CreationArgs&& ... creationArgs)
    {
        return RelicT(std::forward<CreationArgs>(creationArgs)...);
    }

    template<class RelicT>
    RelicT* ReliquaryRelics::PushNewRelic(RelicT&& relic, RelicStructure additionalStructure)
    {
        const auto id = AdvanceID();
        relic.id = id;

        auto& batchSource = batchSources.Required<RelicT>();
        auto added = batchSource.Add(std::move(relic));

        SetupNewInternals(
            id,
            OpennessFor<RelicT>(),
            LocalityFor<RelicT>(),
            HasScribe<RelicT>(),
            TypeFor<RelicT>(),
            added);
        auto structure = StructureFrom<shards_for_t<RelicT>>();
        structure.insert(
            structure.end(),
            additionalStructure.begin(),
            additionalStructure.end());
        SatisfyStructure(id, structure);
        added->Initialize(Owner());
        Shards().NotifyCompositesRelicCreate(id, structure);

        Owner().Raise<Created>(HandleFrom(id, TypeFor<RelicT>(), HandleObjectType::Relic));

        return added;
    }

    template<class RelicT>
    Ptr<RelicT> ReliquaryRelics::CreatePtr(RelicID id) const
    {
        return { id, const_cast<Reliquary&>(Owner()) };
    }
}