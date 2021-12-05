#pragma once

#include <vector>

#include "ReliquaryRelicStructures.h"
#include "ReliquarySignals.h"
#include "ReliquaryShards.h"

#include "RelicStructure.h"
#include "RelicMetadata.h"
#include "RelicBatch.h"
#include "HasShouldCreateMethod.h"
#include "Index.h"

#include "CreateData.h"
#include "IsRelic.h"
#include "IsLocal.h"
#include "IsGlobal.h"
#include "LocalityFor.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class Reliquary;

    class ReliquaryRelics
    {
    public:
        template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Index<RelicT> Create(CreateData createData, ConstructorArgs&& ... constructorArgs);

        void Destroy(const TypeName& typeName, RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Destroy(RelicID id);

        void Clear();
        void Clear(const TypeName& typeName);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Clear();

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        Index<RelicT> Find(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        Index<RelicT> Find() const;

        [[nodiscard]] bool Contains(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains() const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const RelicT& relic) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicID IDFor() const;

        [[nodiscard]] bool IsRelicTypeName(const TypeName& typeName) const;
        [[nodiscard]] std::vector<TypeName> AllTypeNames() const;

        [[nodiscard]] size_t Size(const TypeName& typeName) const;

        template<class RelicT>
        void SignalCreation(const Index<RelicT>& index);
    public:
        RelicID nextRelicID = 1;

        using RelicMetadataMap = std::unordered_map<RelicID, RelicMetadata>;
        RelicMetadataMap metadata;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicMetadata* SetupNewMetadata(RelicID id);
        RelicMetadata* SetupNewMetadata(
            RelicID id,
            Locality locality,
            bool shouldSerializeBinary,
            bool shouldSerializeJson,
            Type type = {});
        void DestroyMetadata(RelicID id);
        [[nodiscard]] RelicMetadata* MetadataFor(RelicID id);
        [[nodiscard]] const RelicMetadata* MetadataFor(RelicID id) const;

        void SatisfyStructure(RelicID id, const RelicStructure& structure);

        [[nodiscard]] bool WillDestroy(RelicMetadata* metadata) const;
        void Destroy(RelicMetadata& metadata);

        [[nodiscard]] std::vector<RelicID> AllIDs() const;

        [[nodiscard]] RelicID NextID() const;
        RelicID AdvanceID();
        
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindStorage(RelicID id);
    public:
        class LocalHandlerBase : public KnownPolymorphicSerializer
        {
        public:
            const TypeName typeName;
        public:
            virtual ~LocalHandlerBase() = 0;

            virtual RelicBatchSourceBase& BatchSource() = 0;

            virtual void Destroy(RelicID id, ReliquaryRelics& relics) = 0;
            virtual void Clear(ReliquaryRelics& relics) = 0;

            virtual void SignalAllCreated(Reliquary& reliquary, ReliquaryRelics& relics) = 0;
        public:
            [[nodiscard]] TypeName MainType() const override;
        protected:
            explicit LocalHandlerBase(const TypeName& typeName);
        };

        template<class RelicT>
        class LocalHandler final : public LocalHandlerBase
        {
        public:
            Arca::BatchSource<RelicT> batchSource;
        public:
            explicit LocalHandler(Reliquary& reliquary, ReliquaryShards& shards);

            RelicBatchSourceBase& BatchSource() override;

            void Destroy(RelicID id, ReliquaryRelics& relics) override;
            void Clear(ReliquaryRelics& relics) override;

            void SignalAllCreated(Reliquary& reliquary, ReliquaryRelics& relics) override;

            [[nodiscard]] bool WillBinarySerialize() const override;
            [[nodiscard]] bool WillJsonSerialize() const override;
            void Serialize(Inscription::Archive::Binary& archive) override;
            void Serialize(const std::string& name, Inscription::Archive::Json& archive) override;
            [[nodiscard]] std::vector<Inscription::Type> InscriptionTypes(Inscription::Archive::Binary& archive) const override;
            [[nodiscard]] std::vector<Inscription::Type> InscriptionTypes(Inscription::Archive::Json& archive) const override;
        };

        using LocalHandlerPtr = std::unique_ptr<LocalHandlerBase>;
        using LocalHandlerList = std::vector<LocalHandlerPtr>;
        LocalHandlerList localHandlers;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void CreateLocalHandler();
        [[nodiscard]] LocalHandlerBase* FindLocalHandler(const TypeName& typeName) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] LocalHandler<RelicT>* FindLocalHandler() const;

        [[nodiscard]] RelicBatchSourceBase* FindBatchSource(const TypeName& typeName) const;
        template<class ObjectT, std::enable_if_t<is_relic_v<ObjectT>, int> = 0>
        [[nodiscard]] BatchSource<ObjectT>* FindBatchSource() const;

        [[nodiscard]] RelicBatchSourceBase& RequiredBatchSource(const TypeName& typeName) const;
        template<class ObjectT, std::enable_if_t<is_relic_v<ObjectT>, int> = 0>
        [[nodiscard]] BatchSource<ObjectT>& RequiredBatchSource() const;

        template<class ObjectT, std::enable_if_t<is_relic_v<ObjectT>, int> = 0>
        [[nodiscard]] Arca::Batch<ObjectT> Batch() const;
    public:
        class GlobalHandlerBase : public KnownPolymorphicSerializer
        {
        public:
            const TypeName typeName;
            RelicID id;
        public:
            virtual ~GlobalHandlerBase() = 0;
        public:
            [[nodiscard]] virtual void* Storage() const = 0;

            [[nodiscard]] TypeName MainType() const override;
        protected:
            explicit GlobalHandlerBase(const TypeName& typeName, RelicID id);
        };

        template<class RelicT>
        class GlobalHandler final : public GlobalHandlerBase
        {
        public:
            std::shared_ptr<RelicT> storage;
        public:
            explicit GlobalHandler(ReliquaryRelics& owner, std::shared_ptr<RelicT>&& storage, RelicID id);

            [[nodiscard]] void* Storage() const override;

            [[nodiscard]] bool WillBinarySerialize() const override;
            [[nodiscard]] bool WillJsonSerialize() const override;
            void Serialize(Inscription::Archive::Binary& archive) override;
            void Serialize(const std::string& name, Inscription::Archive::Json& archive) override;
            [[nodiscard]] std::vector<Inscription::Type> InscriptionTypes(Inscription::Archive::Binary& archive) const override;
            [[nodiscard]] std::vector<Inscription::Type> InscriptionTypes(Inscription::Archive::Json& archive) const override;
        private:
            ReliquaryRelics* owner = nullptr;
        };

        using GlobalHandlerPtr = std::unique_ptr<GlobalHandlerBase>;
        using GlobalHandlerList = std::vector<GlobalHandlerPtr>;
        GlobalHandlerList globalHandlers;

        template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void CreateGlobalHandler(ConstructorArgs&& ... constructorArgs);
        [[nodiscard]] GlobalHandlerBase* FindGlobalHandler(const TypeName& typeName) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] GlobalHandler<RelicT>* FindGlobalHandler() const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindGlobalStorage();
    public:
        ReliquaryRelics(const ReliquaryRelics& arg) = delete;
        ReliquaryRelics& operator=(const ReliquaryRelics& arg) = delete;
    private:
        template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        bool ShouldCreate(ConstructorArgs& ... constructorArgs);

        template<class RelicT>
        void ValidateCreate(CreateData createData);
        
        template<class T, class... ConstructorArgs>
        std::shared_ptr<T> CreateGlobalImpl(RelicInit init, ConstructorArgs&& ... constructorArgs);
    private:
        Reliquary* owner;
        ReliquaryRelicStructures* relicStructures;
        ReliquaryShards* shards;
        ReliquarySignals* signals;

        const std::string objectTypeName = "relic";
    private:
        explicit ReliquaryRelics(
            Reliquary& owner,
            ReliquaryRelicStructures& relicStructures,
            ReliquaryShards& shards,
            ReliquarySignals& signals);
        ReliquaryRelics(ReliquaryRelics&& arg) noexcept = default;
        friend Reliquary;
    };

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::Create(CreateData createData, ConstructorArgs&& ... constructorArgs)
    {
        if (!ShouldCreate<RelicT>(constructorArgs...))
            return {};

        ValidateCreate<RelicT>(createData);

        RelicID id;
        if (createData.id)
        {
            id = *createData.id;
            if (NextID() == id)
                AdvanceID();
        }
        else
            id = AdvanceID();

        const auto structure = !createData.structure
            ? RelicStructure{}
            : std::holds_alternative<RelicStructure>(*createData.structure)
            ? std::get<RelicStructure>(*createData.structure)
            : relicStructures->RequiredRelicStructure(std::get<std::string>(*createData.structure));

        const auto metadata = SetupNewMetadata<RelicT>(id);

        auto& batchSource = RequiredBatchSource<RelicT>();
        auto added = batchSource.Create(id, std::forward<ConstructorArgs>(constructorArgs)...);

        metadata->storage = added;

        SatisfyStructure(id, structure);

        const auto index = Find<RelicT>(id);

        SignalCreation(index);
        
        return index;
    }
    
    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::Destroy(RelicID id)
    {
        const auto metadata = MetadataFor(id);
        if (!WillDestroy(metadata))
            return;

        auto index = Find<RelicT>(id);
        signals->Raise(DestroyingKnown<RelicT>{index});
        Destroy(*metadata);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::Clear()
    {
        auto& batchSource = RequiredBatchSource<RelicT>();
        auto iterator = batchSource.begin();
        while (iterator != batchSource.end())
        {
            Destroy<RelicT>(iterator->first);
            iterator = batchSource.begin();
        }
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_local_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::Find(RelicID id) const
    {
        auto handler = FindLocalHandler<RelicT>();
        return handler ? Index<RelicT>{ id, *owner, handler->batchSource.Find(id) } : Index<RelicT>{};
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    Index<RelicT> ReliquaryRelics::Find() const
    {
        const auto handler = FindGlobalHandler<RelicT>();
        return handler ? Index<RelicT>{ *owner, handler->storage } : Index<RelicT>{};
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_local_v<RelicT>, int>>
    bool ReliquaryRelics::Contains(RelicID id) const
    {
        const auto metadata = MetadataFor(id);
        return metadata && metadata->type.name == TypeFor<RelicT>().name;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_global_v<RelicT>, int>>
    bool ReliquaryRelics::Contains() const
    {
        const auto type = TypeFor<RelicT>();
        const auto handler = FindGlobalHandler<RelicT>();
        return handler != nullptr;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_local_v<RelicT>, int>>
    RelicID ReliquaryRelics::IDFor(const RelicT& relic) const
    {
        return relic.ID();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>&& is_global_v<RelicT>, int>>
    RelicID ReliquaryRelics::IDFor() const
    {
        auto found = FindGlobalHandler<RelicT>();
        return found == nullptr ? nullRelicID : found->id;
    }

    template<class RelicT>
    void ReliquaryRelics::SignalCreation(const Index<RelicT>& index)
    {
        signals->Raise(Created{ Handle{ index.ID(), TypeFor<RelicT>() } });
        signals->Raise(CreatedKnown<RelicT>{index});
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
        return batchSource.Find(id).lock().get();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    void ReliquaryRelics::CreateLocalHandler()
    {
        localHandlers.push_back(std::make_unique<LocalHandler<RelicT>>(*owner, *shards));
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    ReliquaryRelics::LocalHandler<RelicT>* ReliquaryRelics::FindLocalHandler() const
    {
        auto found = FindLocalHandler(TypeFor<RelicT>().name);
        return found == nullptr ? nullptr : static_cast<LocalHandler<RelicT>*>(found);
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
    ReliquaryRelics::LocalHandler<RelicT>::LocalHandler(Reliquary& reliquary, ReliquaryShards& shards) :
        LocalHandlerBase(TypeFor<RelicT>().name), batchSource(reliquary, shards)
    {}

    template<class RelicT>
    RelicBatchSourceBase& ReliquaryRelics::LocalHandler<RelicT>::BatchSource()
    {
        return batchSource;
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::Destroy(RelicID id, ReliquaryRelics& relics)
    {
        relics.Destroy<RelicT>(id);
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::Clear(ReliquaryRelics& relics)
    {
        relics.Clear<RelicT>();
    }

    template<class RelicT>
    void ReliquaryRelics::LocalHandler<RelicT>::SignalAllCreated(Reliquary& reliquary, ReliquaryRelics& relics)
    {
        for (auto& relic : batchSource)
            relics.SignalCreation(relics.Find<RelicT>(relic.first));
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
    ReliquaryRelics::GlobalHandler<RelicT>::GlobalHandler(ReliquaryRelics& owner, std::shared_ptr<RelicT>&& storage, RelicID id) :
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

        const auto metadata = SetupNewMetadata<RelicT>(id);

        auto relic = CreateGlobalImpl<RelicT>(
            RelicInit{ id, *owner, *shards }, std::forward<ConstructorArgs>(constructorArgs)...);

        globalHandlers.push_back(
            std::make_unique<GlobalHandler<RelicT>>(*this, std::move(relic), id));

        metadata->storage = relic.get();
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    ReliquaryRelics::GlobalHandler<RelicT>* ReliquaryRelics::FindGlobalHandler() const
    {
        auto found = FindGlobalHandler(TypeFor<RelicT>().name);
        return found == nullptr ? nullptr : static_cast<GlobalHandler<RelicT>*>(found);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* ReliquaryRelics::FindGlobalStorage()
    {
        auto found = FindGlobalHandler<RelicT>();
        return found == nullptr ? nullptr : static_cast<RelicT*>(found->storage.get());
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    bool ReliquaryRelics::ShouldCreate(ConstructorArgs& ... constructorArgs)
    {
        if constexpr (has_should_create_method_v<RelicT>)
            return Traits<RelicT>::ShouldCreate(*owner, constructorArgs...);
        else
            return true;
    }
    
    template<class RelicT>
    void ReliquaryRelics::ValidateCreate(CreateData createData)
    {
        if (createData.id)
            if (Contains(*createData.id))
                throw CannotCreate(objectTypeName, TypeFor<RelicT>(), typeid(RelicT));
    }

    template<class T, class... ConstructorArgs>
    std::shared_ptr<T> ReliquaryRelics::CreateGlobalImpl(RelicInit init, ConstructorArgs&& ... constructorArgs)
    {
        if constexpr (std::is_constructible_v<T, RelicInit, ConstructorArgs...>)
            return std::make_shared<T>(init, std::forward<ConstructorArgs>(constructorArgs)...);
        else
            return std::make_shared<T>(std::forward<ConstructorArgs>(constructorArgs)...);
    }
}
