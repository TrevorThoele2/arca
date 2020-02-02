#pragma once

#include "ReliquaryRelics.h"
#include "Reliquary.h"

#include "Destroy.h"
#include "Created.h"

#include "OpennessFor.h"
#include "LocalityFor.h"

#include "AsHandle.h"

#include "HasScribe.h"

namespace Arca
{
    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> ReliquaryRelics::Create(ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...))
            return {};

        auto pushed = PushNewRelic<RelicT>({}, std::forward<ConstructorArgs>(constructorArgs)...);
        return CreateIndex<RelicT>(pushed->ID());
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> ReliquaryRelics::CreateWith(const RelicStructure& structure, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...))
            return {};

        auto pushed = PushNewRelic<RelicT>(structure, std::forward<ConstructorArgs>(constructorArgs)...);
        return CreateIndex<RelicT>(pushed->ID());
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> ReliquaryRelics::CreateWith(const std::string& structureName, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...))
            return {};

        auto structure = RelicStructures().RequiredRelicStructure(structureName);
        auto pushed = PushNewRelic<RelicT>(structure, std::forward<ConstructorArgs>(constructorArgs)...);
        return CreateIndex<RelicT>(pushed->ID());
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> ReliquaryRelics::CreateChild(const Handle& parent, ConstructorArgs&& ... constructorArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });
        auto child = Create<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...);
        Parent(parent, AsHandle(*child));
        return child;
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const RelicStructure& structure, ConstructorArgs&& ... constructorArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });
        auto child = CreateWith<RelicT>(structure, std::forward<ConstructorArgs>(constructorArgs)...);
        Parent(parent, AsHandle(*child));
        return child;
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicIndex<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const std::string& structureName, ConstructorArgs&& ... constructorArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });
        auto child = CreateWith<RelicT>(structureName, std::forward<ConstructorArgs>(constructorArgs)...);
        Parent(parent, AsHandle(*child));
        return child;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::Destroy(RelicID id)
    {
        const auto metadata = MetadataFor(id);
        if (!WillDestroy(metadata))
            return;

        auto index = CreateIndex<RelicT>(id);
        Owner().Raise<DestroyingKnown<RelicT>>(index);
        Destroy(*metadata);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::Clear()
    {
        auto& batchSource = RequiredBatchSource<RelicT>();
        auto iterator = batchSource.begin();
        while(iterator != batchSource.end())
        {
            Destroy<RelicT>(iterator->ID());
            iterator = batchSource.begin();
        }
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    bool ReliquaryRelics::Contains(RelicID id) const
    {
        const auto metadata = MetadataFor(id);
        return metadata && metadata->locality == Locality::Local;
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
            return 0;

        return found->id;
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
        localHandlers.push_back(std::make_unique<LocalHandler<RelicT>>(Owner()));
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
        if (batchSource == nullptr)
            return nullptr;

        return static_cast<BatchSource<ObjectT>*>(batchSource);
    }

    template<class ObjectT, std::enable_if_t<is_relic_v<ObjectT>, int>>
    BatchSource<ObjectT>& ReliquaryRelics::RequiredBatchSource() const
    {
        auto found = FindBatchSource<ObjectT>();
        if (!found)
        {
            const auto type = TypeFor<ObjectT>();
            throw NotRegistered(type, typeid(ObjectT));
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
        reliquary.Do<Arca::Destroy<RelicT>>(id);
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::Clear()
    {
        batchSource.Clear();
    }

    template<class RelicT>
    bool ReliquaryRelics::LocalHandler<RelicT>::WillSerialize() const
    {
        return HasScribe<RelicT>();
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::Serialize(Inscription::BinaryArchive& archive)
    {
        if (!HasScribe<RelicT>())
            return;

        archive(batchSource);
    }

    template<class RelicT>
    std::vector<::Inscription::Type> ReliquaryRelics::LocalHandler<RelicT>::InscriptionTypes(Inscription::BinaryArchive& archive) const
    {
        return ::Inscription::InputTypesFor<RelicT>(archive);
    }

    template<class RelicT>
    ReliquaryRelics::GlobalHandler<RelicT>::GlobalHandler(ReliquaryRelics& owner, std::shared_ptr<void>&& storage, RelicID id) :
        GlobalHandlerBase(TypeFor<RelicT>().name, std::move(storage), id), owner(&owner)
    {}

    template<class RelicT>
    bool ReliquaryRelics::GlobalHandler<RelicT>::WillSerialize() const
    {
        return HasScribe<RelicT>();
    }

    template<class RelicT>
    void ReliquaryRelics::GlobalHandler<RelicT>::Serialize(Inscription::BinaryArchive& archive)
    {
        archive(*reinterpret_cast<RelicT*>(storage.get()));
    }

    template<class RelicT>
    std::vector<::Inscription::Type> ReliquaryRelics::GlobalHandler<RelicT>::InscriptionTypes(Inscription::BinaryArchive& archive) const
    {
        return ::Inscription::InputTypesFor<RelicT>(archive);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::CreateGlobalHandler(ConstructorArgs&& ... constructorArgs)
    {
        const auto type = TypeFor<RelicT>();
        const auto id = AdvanceID();
        auto metadata = SetupNewInternals(
            id,
            OpennessFor<RelicT>(),
            LocalityFor<RelicT>(),
            HasScribe<RelicT>(),
            Type(type.name));
        const auto storage = std::make_shared<RelicT>(RelicInitialization{ id, Owner() }, std::forward<ConstructorArgs>(constructorArgs)...);
        globalHandlers.push_back(
            std::make_unique<GlobalHandler<RelicT>>(*this, storage, id));
        auto relic = reinterpret_cast<RelicT*>(globalHandlers.back()->storage.get());
        metadata->storage = relic;
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
        if (found == nullptr)
            return nullptr;

        return static_cast<RelicT*>(found->storage.get());
    }

    template<class T>
    T ReliquaryRelics::FindGlobalComputation()
    {
        const auto computationFound = globalComputationMap.find(typeid(T));
        if (computationFound != globalComputationMap.end())
            return std::any_cast<T>(computationFound->second(Owner()));

        throw Arca::NotRegistered("global computation", typeid(T));
    }

    template<
        class RelicT,
        class... ConstructorArgs,
        std::enable_if_t<is_relic_v<RelicT> && has_should_create_method_v<RelicT>, int>>
        bool ReliquaryRelics::ShouldCreate(ConstructorArgs&& ... constructorArgs)
    {
        return Traits<RelicT>::ShouldCreate(Owner(), std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<
        class RelicT,
        class... ConstructorArgs,
        std::enable_if_t<is_relic_v<RelicT> && !has_should_create_method_v<RelicT>, int>>
        bool ReliquaryRelics::ShouldCreate(ConstructorArgs&& ...)
    {
        return true;
    }

    template<class RelicT, class... ConstructorArgs>
    RelicT* ReliquaryRelics::PushNewRelic(RelicStructure structure, ConstructorArgs&& ... constructorArgs)
    {
        const auto id = AdvanceID();

        auto metadata = SetupNewInternals(
            id,
            OpennessFor<RelicT>(),
            LocalityFor<RelicT>(),
            HasScribe<RelicT>(),
            TypeFor<RelicT>());

        auto& batchSource = RequiredBatchSource<RelicT>();
        auto added = batchSource.Add(RelicT{ RelicInitialization{id, Owner()}, std::forward<ConstructorArgs>(constructorArgs)... });

        metadata->storage = added;
        SatisfyStructure(id, structure);

        Owner().Raise<Created>(HandleFrom(id, TypeFor<RelicT>(), HandleObjectType::Relic));
        Owner().Raise<CreatedKnown<RelicT>>(CreateIndex<RelicT>(id));

        return added;
    }

    template<class T>
    auto ReliquaryRelics::CreateIndex(RelicID id) const
    {
        return ToIndex<T>(id, const_cast<Reliquary&>(Owner()));
    }
}