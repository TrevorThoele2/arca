#pragma once

#include "ReliquaryRelics.h"
#include "Created.h"
#include "Initialize.h"
#include "PostConstruct.h"
#include "AsHandle.h"

namespace Arca
{
    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::Create(InitializeArgs&& ... initializeArgs)
    {
        if (!ShouldCreate<RelicT>(std::forward<InitializeArgs>(initializeArgs)...))
            return {};

        RelicT relic;
        auto pushed = PushNewRelic(std::move(relic), {}, std::forward<InitializeArgs>(initializeArgs)...);
        return CreatePtr<RelicT>(pushed->ID());
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateWith(const RelicStructure& structure, InitializeArgs&& ... initializeArgs)
    {
        if (!ShouldCreate<RelicT>(std::forward<InitializeArgs>(initializeArgs)...))
            return {};

        RelicT relic;
        auto pushed = PushNewRelic(std::move(relic), structure);
        return CreatePtr<RelicT>(pushed->ID());
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateWith(const std::string& structureName, InitializeArgs&& ... initializeArgs)
    {
        if (!ShouldCreate<RelicT>(std::forward<InitializeArgs>(initializeArgs)...))
            return {};

        RelicT relic;
        auto structure = RelicStructures().RequiredRelicStructure(structureName);
        auto pushed = PushNewRelic(std::move(relic), structure);
        return CreatePtr<RelicT>(pushed->ID());
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateChild(const Handle& parent, InitializeArgs&& ... initializeArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });
        auto child = Create<RelicT>(std::forward<InitializeArgs>(initializeArgs)...);
        Parent(parent, AsHandle(*child));
        return child;
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const RelicStructure& structure, InitializeArgs&& ... initializeArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });
        auto child = CreateWith<RelicT>(structure, std::forward<InitializeArgs>(initializeArgs)...);
        Parent(parent, AsHandle(*child));
        return child;
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Ptr<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const std::string& structureName, InitializeArgs&& ... initializeArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });
        auto child = CreateWith<RelicT>(structureName, std::forward<InitializeArgs>(initializeArgs)...);
        Parent(parent, AsHandle(*child));
        return child;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::Clear()
    {
        auto& batchSource = batchSources.Required<RelicT>();
        for (auto loop = batchSource.begin(); loop != batchSource.end();)
        {
            auto next = std::next(loop);
            Owner().Destroy(AsHandle(*loop));
            loop = next;
        }
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

    template<class RelicT, std::enable_if_t<is_local_relic_v<RelicT>, int>>
    RelicID ReliquaryRelics::IDFor(const RelicT& relic) const
    {
        return relic.ID();
    }

    template<class RelicT, std::enable_if_t<is_global_relic_v<RelicT>, int>>
    RelicID ReliquaryRelics::IDFor() const
    {
        auto found = globalMap.find(TypeFor<RelicT>());
        if (found == globalMap.end())
            return 0;

        return found->second->id;
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
        class... InitializeArgs,
        std::enable_if_t<is_relic_v<RelicT> && has_should_create_method_v<RelicT>, int>>
        bool ReliquaryRelics::ShouldCreate(InitializeArgs&& ... initializeArgs)
    {
        return Traits<RelicT>::ShouldCreate(Owner(), std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<
        class RelicT,
        class... InitializeArgs,
        std::enable_if_t<is_relic_v<RelicT> && !has_should_create_method_v<RelicT>, int>>
        bool ReliquaryRelics::ShouldCreate(InitializeArgs&& ...)
    {
        return true;
    }

    template<class RelicT, class... InitializeArgs>
    RelicT* ReliquaryRelics::PushNewRelic(
        RelicT&& relic, RelicStructure additionalStructure, InitializeArgs&& ... initializeArgs)
    {
        const auto id = AdvanceID();
        relic.id = id;
        relic.owner = &Owner();

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
        PostConstruct(*added);
        Initialize(*added, std::forward<InitializeArgs>(initializeArgs)...);

        Owner().Raise<Created>(HandleFrom(id, TypeFor<RelicT>(), HandleObjectType::Relic));

        return added;
    }

    template<class RelicT>
    Ptr<RelicT> ReliquaryRelics::CreatePtr(RelicID id) const
    {
        return { id, const_cast<Reliquary&>(Owner()) };
    }
}