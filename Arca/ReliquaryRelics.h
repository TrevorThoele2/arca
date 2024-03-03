#pragma once

#include "ReliquaryComponent.h"

#include <vector>
#include <any>

#include "OpenRelic.h"
#include "RelicStructure.h"
#include "RelicMetadata.h"
#include "RelicBatch.h"
#include "HasShouldCreateMethod.h"

#include "IsRelic.h"
#include "IsLocal.h"
#include "IsGlobal.h"

#include "RelicIndex.h"
#include "ComputedIndex.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class Reliquary;

    class ReliquaryRelics : public ReliquaryComponent
    {
    public:
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> Create(InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateWith(const RelicStructure& structure, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateWith(const std::string& structureName, InitializeArgs&& ... initializeArgs);

        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateChild(const Handle& parent, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateChildWith(const Handle& parent, const RelicStructure& structure, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicIndex<RelicT> CreateChildWith(const Handle& parent, const std::string& structureName, InitializeArgs&& ... initializeArgs);

        void Destroy(const TypeName& typeName, RelicID id);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Destroy(RelicID id);

        void Clear(const TypeName& typeName);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Clear();

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] bool Contains() const;

        [[nodiscard]] std::optional<Handle> ParentOf(const Handle& child) const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const RelicT& relic) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        [[nodiscard]] RelicID IDFor() const;
    public:
        struct RelicPrototype
        {
            RelicID id;
            Openness openness;
        };
    public:
        using RelicMetadataList = std::vector<RelicMetadata>;
        RelicMetadataList metadataList;

        RelicID nextRelicID = 1;

        void SetupNewInternals(
            RelicID id,
            Openness openness,
            Locality locality,
            bool shouldSerialize,
            Type type = {},
            void* storage = nullptr);
        void DestroyMetadata(RelicID id);
        [[nodiscard]] RelicMetadata* MetadataFor(RelicID id);
        [[nodiscard]] const RelicMetadata* MetadataFor(RelicID id) const;

        void SatisfyStructure(RelicID id, const RelicStructure& structure);

        [[nodiscard]] bool WillDestroy(RelicMetadata* metadata) const;
        void Destroy(RelicMetadata& metadata);

        [[nodiscard]] std::vector<RelicID> AllIDs() const;

        [[nodiscard]] RelicID NextID() const;
        [[nodiscard]] RelicID AdvanceID();

        [[nodiscard]] bool CanModifyShards(RelicID id) const;
        void ShardModificationRequired(RelicID id) const;

        void ThrowIfCannotParent(const Handle& parent, RelicPrototype child);
        void Parent(const Handle& parent, const Handle& child);

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

            virtual void Destroy(RelicID id, Reliquary& reliquary) = 0;
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
            explicit LocalHandler(Reliquary& reliquary);

            RelicBatchSourceBase& BatchSource() override;

            void Destroy(RelicID id, Reliquary& reliquary) override;

            bool WillSerialize() const override;
            void Serialize(Inscription::BinaryArchive& archive) override;
            [[nodiscard]] std::vector<::Inscription::Type> InscriptionTypes(Inscription::BinaryArchive& archive) const override;
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
            std::shared_ptr<void> storage;
            RelicID id;
        public:
            virtual ~GlobalHandlerBase() = 0;

            virtual void PostConstruct() = 0;
        public:
            [[nodiscard]] TypeName MainType() const override;
        protected:
            explicit GlobalHandlerBase(const TypeName& typeName, std::shared_ptr<void>&& storage, RelicID id);
        };

        template<class RelicT>
        class GlobalHandler final : public GlobalHandlerBase
        {
        public:
            explicit GlobalHandler(ReliquaryRelics& owner, std::shared_ptr<void>&& storage, RelicID id);

            void PostConstruct() override;

            bool WillSerialize() const override;
            void Serialize(Inscription::BinaryArchive& archive) override;
            [[nodiscard]] std::vector<::Inscription::Type> InscriptionTypes(Inscription::BinaryArchive& archive) const override;
        private:
            ReliquaryRelics* owner = nullptr;
        };

        using GlobalHandlerPtr = std::unique_ptr<GlobalHandlerBase>;
        using GlobalHandlerList = std::vector<GlobalHandlerPtr>;
        GlobalHandlerList globalHandlers;

        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void CreateGlobalHandler(InitializeArgs&& ... initializeArgs);
        [[nodiscard]] GlobalHandlerBase* FindGlobalHandler(const TypeName& typeName) const;
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] GlobalHandler<RelicT>* FindGlobalHandler() const;

        using GlobalComputationTransformation = std::function<std::any(Reliquary&)>;
        using GlobalComputationMap = std::unordered_map<std::type_index, GlobalComputationTransformation>;
        GlobalComputationMap globalComputationMap;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindGlobalStorage();
        template<class T>
        T FindGlobalComputation();
    public:
        ReliquaryRelics(const ReliquaryRelics& arg) = delete;
        ReliquaryRelics& operator=(const ReliquaryRelics& arg) = delete;
    private:
        template<
            class RelicT,
            class... InitializeArgs,
            std::enable_if_t<is_relic_v<RelicT> && has_should_create_method_v<RelicT>, int> = 0>
            bool ShouldCreate(InitializeArgs&& ... initializeArgs);
        template<
            class RelicT,
            class... InitializeArgs,
            std::enable_if_t<is_relic_v<RelicT> && !has_should_create_method_v<RelicT>, int> = 0>
            bool ShouldCreate(InitializeArgs&& ... initializeArgs);

        template<class RelicT, class... InitializeArgs>
        RelicT* PushNewRelic(RelicT&& relic, RelicStructure structure, InitializeArgs&& ... initializeArgs);
    private:
        RelicMetadata& ValidateParentForParenting(const Handle& parent);
    private:
        template<class T>
        auto CreateIndex(RelicID id) const;
    private:
        explicit ReliquaryRelics(Reliquary& owner);
        ReliquaryRelics(ReliquaryRelics&& arg) noexcept = default;
        friend Reliquary;
    };
}
