#pragma once

#include "ReliquaryRelics.h"
#include "Reliquary.h"
#include "ReliquaryException.h"

#include "Destroy.h"
#include "Created.h"

#include "LocalityFor.h"

#include "AsHandle.h"

#include "HasScribe.h"

namespace Arca
{
    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::Create(ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        const auto id = AdvanceID();
        SetupNewMetadata<RelicT>(id);
        return FinishNewRelic<RelicT>({}, id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateWith(const RelicStructure& structure, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        const auto id = AdvanceID();
        SetupNewMetadata<RelicT>(id);
        return FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateWith(const std::string& structureName, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        const auto id = AdvanceID();
        SetupNewMetadata<RelicT>(id);
        auto structure = relicStructures->RequiredRelicStructure(structureName);
        return FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::IdentifiedCreate(RelicID id, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        if (Contains(id))
            throw CannotCreate(objectTypeName, TypeFor<RelicT>(), typeid(RelicT));
        if (NextID() == id)
            AdvanceID();

        SetupNewMetadata<RelicT>(id);
        return FinishNewRelic<RelicT>({}, id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::IdentifiedCreateWith(RelicID id, const RelicStructure& structure, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        if (Contains(id))
            throw CannotCreate(objectTypeName, TypeFor<RelicT>(), typeid(RelicT));
        if (NextID() == id)
            AdvanceID();

        SetupNewMetadata<RelicT>(id);
        return FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::IdentifiedCreateWith(RelicID id, const std::string& structureName, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        if (Contains(id))
            throw CannotCreate(objectTypeName, TypeFor<RelicT>(), typeid(RelicT));
        if (NextID() == id)
            AdvanceID();

        SetupNewMetadata<RelicT>(id);
        auto structure = relicStructures->RequiredRelicStructure(structureName);
        return FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateChild(const Handle& parent, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        ThrowIfCannotParent(parent, RelicPrototype{ NextID() });

        const auto id = AdvanceID();
        SetupNewMetadata<RelicT>(id);
        Parent(parent, Handle{ id, TypeFor<RelicT>() });
        auto index = FinishNewRelic<RelicT>({}, id, std::forward<ConstructorArgs>(constructorArgs)...);
        owner->Raise(RelicParented{ parent, AsHandle(index) });
        return index;
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const RelicStructure& structure, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        ThrowIfCannotParent(parent, RelicPrototype{ NextID() });

        const auto id = AdvanceID();
        SetupNewMetadata<RelicT>(id);
        Parent(parent, Handle{ id, TypeFor<RelicT>() });
        auto index = FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
        owner->Raise(RelicParented{ parent, AsHandle(index) });
        return index;
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const std::string& structureName, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        ThrowIfCannotParent(parent, RelicPrototype{ NextID() });

        const auto id = AdvanceID();
        SetupNewMetadata<RelicT>(id);
        Parent(parent, Handle(id, TypeFor<RelicT>()));
        auto structure = relicStructures->RequiredRelicStructure(structureName);
        auto index = FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
        owner->Raise(RelicParented{ parent, AsHandle(index) });
        return index;
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::IdentifiedCreateChild(RelicID id, const Handle& parent, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        ThrowIfCannotParent(parent, RelicPrototype{ NextID() });

        if (Contains(id))
            throw CannotCreate(objectTypeName, TypeFor<RelicT>(), typeid(RelicT));
        if (NextID() == id)
            AdvanceID();

        SetupNewMetadata<RelicT>(id);
        Parent(parent, Handle{ id, TypeFor<RelicT>() });
        auto index = FinishNewRelic<RelicT>({}, id, std::forward<ConstructorArgs>(constructorArgs)...);
        owner->Raise(RelicParented{ parent, AsHandle(index) });
        return index;
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::IdentifiedCreateChildWith(RelicID id, const Handle& parent, const RelicStructure& structure, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        ThrowIfCannotParent(parent, RelicPrototype{ NextID() });

        if (Contains(id))
            throw CannotCreate(objectTypeName, TypeFor<RelicT>(), typeid(RelicT));
        if (NextID() == id)
            AdvanceID();

        SetupNewMetadata<RelicT>(id);
        Parent(parent, Handle{ id, TypeFor<RelicT>() });
        auto index = FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
        owner->Raise(RelicParented{parent, AsHandle(index)});
        return index;
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::IdentifiedCreateChildWith(RelicID id, const Handle& parent, const std::string& structureName, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        ThrowIfCannotParent(parent, RelicPrototype{ NextID() });

        if (Contains(id))
            throw CannotCreate(objectTypeName, TypeFor<RelicT>(), typeid(RelicT));
        if (NextID() == id)
            AdvanceID();

        SetupNewMetadata<RelicT>(id);
        Parent(parent, Handle(id, TypeFor<RelicT>()));
        auto structure = relicStructures->RequiredRelicStructure(structureName);
        auto index = FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
        owner->Raise(RelicParented{ parent, AsHandle(*index) });
        return index;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::Destroy(RelicID id)
    {
        const auto metadata = MetadataFor(id);
        if (!WillDestroy(metadata))
            return;

        auto index = Index<RelicT>(id, *owner);
        owner->Raise(DestroyingKnown<RelicT>{index});
        Destroy(*metadata);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::Clear()
    {
        auto& batchSource = RequiredBatchSource<RelicT>();
        auto iterator = batchSource.begin();
        while(iterator != batchSource.end())
        {
            Destroy<RelicT>(iterator->first);
            iterator = batchSource.begin();
        }
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    bool ReliquaryRelics::Contains(RelicID id) const
    {
        const auto metadata = MetadataFor(id);
        return metadata && metadata->type.name == TypeFor<RelicT>().name;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    bool ReliquaryRelics::Contains() const
    {
        const auto type = TypeFor<RelicT>();
        const auto found = FindGlobalHandler<RelicT>();
        return found != nullptr;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    RelicID ReliquaryRelics::IDFor(const RelicT& relic) const
    {
        return relic.ID();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    RelicID ReliquaryRelics::IDFor() const
    {
        auto found = FindGlobalHandler<RelicT>();
        if (found == nullptr)
            return nullRelicID;

        return found->id;
    }

    template<class RelicT>
    void ReliquaryRelics::SignalCreation(const Index<RelicT>& index)
    {
        owner->Raise(Created{ Handle{ index.ID(), TypeFor<RelicT>() }});
        owner->Raise(CreatedKnown<RelicT>{index});
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicMetadata* ReliquaryRelics::SetupNewMetadata(RelicID id)
    {
        return SetupNewMetadata(
            id,
            LocalityFor<RelicT>(),
            HasScribe<RelicT, Inscription::Archive::Binary>(),
            HasScribe<RelicT, Inscription::Archive::Json>(),
            TypeFor<RelicT>());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* ReliquaryRelics::FindStorage(RelicID id)
    {
        const auto metadata = MetadataFor(id);
        if (!metadata)
            return {};

        auto& batchSource = RequiredBatchSource<RelicT>();
        return batchSource.Find(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::CreateLocalHandler()
    {
        localHandlers.push_back(std::make_unique<LocalHandler<RelicT>>(*owner));
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    ReliquaryRelics::LocalHandler<RelicT>* ReliquaryRelics::FindLocalHandler() const
    {
        auto found = FindLocalHandler(TypeFor<RelicT>().name);
        if (found == nullptr)
            return nullptr;

        return static_cast<LocalHandler<RelicT>*>(found);
    }

    template<class ObjectT, std::enable_if_t<is_relic_v<ObjectT>, int>>
    BatchSource<ObjectT>* ReliquaryRelics::FindBatchSource() const
    {
        const auto typeName = TypeFor<ObjectT>().name;
        auto batchSource = FindBatchSource(typeName);
        return batchSource == nullptr ? nullptr : static_cast<BatchSource<ObjectT>*>(batchSource);
    }

    template<class ObjectT, std::enable_if_t<is_relic_v<ObjectT>, int>>
    BatchSource<ObjectT>& ReliquaryRelics::RequiredBatchSource() const
    {
        auto found = FindBatchSource<ObjectT>();
        if (!found)
        {
            const auto type = TypeFor<ObjectT>();
            throw NotRegistered(objectTypeName, type, typeid(ObjectT));
        }

        return *found;
    }

    template<class ObjectT, std::enable_if_t<is_relic_v<ObjectT>, int>>
    Arca::Batch<ObjectT> ReliquaryRelics::Batch() const
    {
        auto& batchSource = RequiredBatchSource<ObjectT>();
        return Arca::Batch<ObjectT>(batchSource);
    }

    template<class RelicT>
    ReliquaryRelics::LocalHandler<RelicT>::LocalHandler(Reliquary& reliquary) :
        LocalHandlerBase(TypeFor<RelicT>().name), batchSource(reliquary)
    {}

    template<class RelicT>
    RelicBatchSourceBase& ReliquaryRelics::LocalHandler<RelicT>::BatchSource()
    {
        return batchSource;
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::Destroy(RelicID id, Reliquary& reliquary)
    {
        reliquary.Do(Arca::Destroy<RelicT>{id});
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::Clear(Reliquary& reliquary)
    {
        reliquary.relics.Clear<RelicT>();
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::SignalAllCreated(Reliquary& reliquary)
    {
        for (auto& relic : batchSource)
            reliquary.relics.SignalCreation(Index<RelicT>(relic.first, reliquary));
    }

    template<class RelicT>
    bool ReliquaryRelics::LocalHandler<RelicT>::WillBinarySerialize() const
    {
        return HasScribe<RelicT, Inscription::Archive::Binary>();
    }

    template<class RelicT>
    bool ReliquaryRelics::LocalHandler<RelicT>::WillJsonSerialize() const
    {
        return HasScribe<RelicT, Inscription::Archive::Json>();
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::Serialize(Inscription::Archive::Binary& archive)
    {
        archive(batchSource);
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::Serialize(const std::string& name, Inscription::Archive::Json& archive)
    {
        archive(name, batchSource);
    }

    template<class RelicT>
    std::vector<Inscription::Type> ReliquaryRelics::LocalHandler<RelicT>::InscriptionTypes(Inscription::Archive::Binary& archive) const
    {
        return Inscription::InputTypesFor<RelicT>(archive);
    }

    template<class RelicT>
    std::vector<Inscription::Type> ReliquaryRelics::LocalHandler<RelicT>::InscriptionTypes(Inscription::Archive::Json& archive) const
    {
        return Inscription::InputTypesFor<RelicT>(archive);
    }

    template<class RelicT>
    ReliquaryRelics::GlobalHandler<RelicT>::GlobalHandler(ReliquaryRelics& owner, std::unique_ptr<RelicT>&& storage, RelicID id) :
        GlobalHandlerBase(TypeFor<RelicT>().name, id), storage(std::move(storage)), owner(&owner)
    {}

    template<class RelicT>
    void* ReliquaryRelics::GlobalHandler<RelicT>::Storage() const
    {
        return storage.get();
    }

    template<class RelicT>
    bool ReliquaryRelics::GlobalHandler<RelicT>::WillBinarySerialize() const
    {
        return HasScribe<RelicT, Inscription::Archive::Binary>();
    }

    template<class RelicT>
    bool ReliquaryRelics::GlobalHandler<RelicT>::WillJsonSerialize() const
    {
        return HasScribe<RelicT, Inscription::Archive::Json>();
    }

    template<class RelicT>
    void ReliquaryRelics::GlobalHandler<RelicT>::Serialize(Inscription::Archive::Binary& archive)
    {
        archive(*reinterpret_cast<RelicT*>(storage.get()));
    }

    template<class RelicT>
    void ReliquaryRelics::GlobalHandler<RelicT>::Serialize(const std::string& name, Inscription::Archive::Json& archive)
    {
        archive(name, *reinterpret_cast<RelicT*>(storage.get()));
    }

    template<class RelicT>
    std::vector<Inscription::Type> ReliquaryRelics::GlobalHandler<RelicT>::InscriptionTypes(Inscription::Archive::Binary& archive) const
    {
        return Inscription::InputTypesFor<RelicT>(archive);
    }

    template<class RelicT>
    std::vector<Inscription::Type> ReliquaryRelics::GlobalHandler<RelicT>::InscriptionTypes(Inscription::Archive::Json& archive) const
    {
        return Inscription::InputTypesFor<RelicT>(archive);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::CreateGlobalHandler(ConstructorArgs&& ... constructorArgs)
    {
        const auto id = AdvanceID();

        SetupNewMetadata<RelicT>(id);

        auto relic = CreateGlobalImpl<RelicT>(
            RelicInit {id, *owner}, std::forward<ConstructorArgs>(constructorArgs)...);

        globalHandlers.push_back(
            std::make_unique<GlobalHandler<RelicT>>(*this, std::move(relic), id));

        MetadataFor(id)->storage = relic.get();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    ReliquaryRelics::GlobalHandler<RelicT>* ReliquaryRelics::FindGlobalHandler() const
    {
        auto found = FindGlobalHandler(TypeFor<RelicT>().name);
        if (found == nullptr)
            return nullptr;

        return static_cast<GlobalHandler<RelicT>*>(found);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* ReliquaryRelics::FindGlobalStorage()
    {
        auto found = FindGlobalHandler<RelicT>();
        return found == nullptr ? nullptr : static_cast<RelicT*>(found->storage.get());
    }

    template<
        class RelicT,
        class... ConstructorArgs,
        std::enable_if_t<is_relic_v<RelicT> && has_should_create_method_v<RelicT>, int>>
    bool ReliquaryRelics::ShouldCreate(ConstructorArgs& ... constructorArgs)
    {
        return Traits<RelicT>::ShouldCreate(*owner, constructorArgs...);
    }

    template<
        class RelicT,
        class... ConstructorArgs,
        std::enable_if_t<is_relic_v<RelicT> && !has_should_create_method_v<RelicT>, int>>
    bool ReliquaryRelics::ShouldCreate(ConstructorArgs& ...)
    {
        return true;
    }

    template<class RelicT, class... ConstructorArgs>
    Index<RelicT> ReliquaryRelics::FinishNewRelic(
        RelicStructure structure, RelicID id, ConstructorArgs&& ... constructorArgs)
    {
        auto& batchSource = RequiredBatchSource<RelicT>();
        auto added = batchSource.Create(id, std::forward<ConstructorArgs>(constructorArgs)...);

        MetadataFor(id)->storage = added;

        SatisfyStructure(id, structure);

        const auto index = Index<RelicT>{ id, *owner };

        SignalCreation(index);

        return index;
    }
}