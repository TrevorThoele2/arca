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
    Index<RelicT> ReliquaryRelics::Create(ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...))
            return {};

        auto id = StartNewRelic<RelicT>();
        return FinishNewRelic<RelicT>({}, id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateWith(const RelicStructure& structure, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...))
            return {};

        auto id = StartNewRelic<RelicT>();
        return FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateWith(const std::string& structureName, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...))
            return {};

        auto id = StartNewRelic<RelicT>();
        auto structure = RelicStructures().RequiredRelicStructure(structureName);
        return FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateChild(const Handle& parent, ConstructorArgs&& ... constructorArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });

        if (!ShouldCreate<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...))
            return {};

        auto id = StartNewRelic<RelicT>();
        Parent(parent, Handle(id, Owner(), TypeFor<RelicT>(), HandleObjectType::Relic));
        auto index = FinishNewRelic<RelicT>({}, id, std::forward<ConstructorArgs>(constructorArgs)...);
        Owner().Raise<RelicParented>({ parent, AsHandle(*index) });
        return index;
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const RelicStructure& structure, ConstructorArgs&& ... constructorArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });

        if (!ShouldCreate<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...))
            return {};

        auto id = StartNewRelic<RelicT>();
        Parent(parent, Handle(id, Owner(), TypeFor<RelicT>(), HandleObjectType::Relic));
        auto index = FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
        Owner().Raise<RelicParented>({ parent, AsHandle(*index) });
        return index;
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::CreateChildWith(
        const Handle& parent, const std::string& structureName, ConstructorArgs&& ... constructorArgs)
    {
        ThrowIfCannotParent(parent, RelicPrototype{ NextID(), OpennessFor<RelicT>() });

        if (!ShouldCreate<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...))
            return {};

        auto id = StartNewRelic<RelicT>();
        Parent(parent, Handle(id, Owner(), TypeFor<RelicT>(), HandleObjectType::Relic));
        auto structure = RelicStructures().RequiredRelicStructure(structureName);
        auto index = FinishNewRelic<RelicT>(structure, id, std::forward<ConstructorArgs>(constructorArgs)...);
        Owner().Raise<RelicParented>({ parent, AsHandle(*index) });
        return index;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::Destroy(RelicID id)
    {
        const auto metadata = MetadataFor(id);
        if (!WillDestroy(metadata))
            return;

        auto index = Index<RelicT>(id, Owner());
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
    ReliquaryRelics::GlobalHandler<RelicT>::GlobalHandler(ReliquaryRelics& owner, std::unique_ptr<RelicT>&& storage, RelicID id) :
        GlobalHandlerBase(TypeFor<RelicT>().name, id), owner(&owner), storage(std::move(storage))
    {}

    template<class RelicT>
    void* ReliquaryRelics::GlobalHandler<RelicT>::Storage() const
    {
        return storage.get();
    }

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

        auto metadata = SetupNewMetadata(
            id,
            OpennessFor<RelicT>(),
            LocalityFor<RelicT>(),
            HasScribe<RelicT>(),
            Type(type.name));

        auto relic = std::make_unique<RelicT>(RelicInit{ id, Owner() }, std::forward<ConstructorArgs>(constructorArgs)...);

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
        if (found == nullptr)
            return nullptr;

        return static_cast<RelicT*>(found->storage.get());
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

    template<class RelicT>
    RelicID ReliquaryRelics::StartNewRelic()
    {
        const auto id = AdvanceID();

        SetupNewMetadata(
            id,
            OpennessFor<RelicT>(),
            LocalityFor<RelicT>(),
            HasScribe<RelicT>(),
            TypeFor<RelicT>());

        return id;
    }

    template<class RelicT, class... ConstructorArgs>
    Index<RelicT> ReliquaryRelics::FinishNewRelic(
        RelicStructure structure, RelicID id, ConstructorArgs&& ... constructorArgs)
    {
        auto& batchSource = RequiredBatchSource<RelicT>();
        auto added = batchSource.Add(
            RelicT{ RelicInit{id, Owner()}, std::forward<ConstructorArgs>(constructorArgs)... });

        MetadataFor(id)->storage = added;

        SatisfyStructure(id, structure);

        const auto index = Index<RelicT>{ id, Owner() };

        Owner().Raise<Created>(HandleFrom(index.ID(), TypeFor<RelicT>(), HandleObjectType::Relic));
        Owner().Raise<CreatedKnown<RelicT>>(index);

        return index;
    }
}