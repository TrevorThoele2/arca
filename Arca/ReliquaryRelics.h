#pragma once

#include "ReliquaryComponent.h"

#include <vector>
#include <any>

#include "OpenRelic.h"
#include "RelicStructure.h"
#include "RelicTraits.h"
#include "RelicMetadata.h"
#include "RelicBatchSource.h"
#include "RelicBatch.h"
#include "HasShouldCreateMethod.h"

#include "Ptr.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class ReliquaryRelics : public ReliquaryComponent
    {
    public:
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> Create(InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateWith(const RelicStructure& structure, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateWith(const std::string& structureName, InitializeArgs&& ... initializeArgs);

        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateChild(const Handle& parent, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateChildWith(const Handle& parent, const RelicStructure& structure, InitializeArgs&& ... initializeArgs);
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateChildWith(const Handle& parent, const std::string& structureName, InitializeArgs&& ... initializeArgs);

        void Destroy(const Handle& handle);

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        void Clear();

        template<class RelicT, std::enable_if_t<is_local_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Ptr<RelicT> Find(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_global_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Ptr<RelicT> Find() const;

        [[nodiscard]] std::optional<Handle> ParentOf(const Handle& child) const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const RelicT& relic) const;
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
        class BatchSources
            : public StorageBatchSources<RelicBatchSourceBase, ReliquaryRelics, BatchSources, is_relic>
        { 
        public:
            template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
            [[nodiscard]] Map& MapFor();
            template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
            [[nodiscard]] const Map& MapFor() const;
        private:
            explicit BatchSources(ReliquaryRelics& owner);
            friend ReliquaryRelics;
        } batchSources = BatchSources(*this);

        KnownPolymorphicSerializerList serializers;
    public:
        struct StoredGlobal
        {
            std::shared_ptr<void> storage;
            RelicID id;
        };

        using GlobalMap = std::unordered_map<TypeName, StoredGlobal>;
        GlobalMap globalMap;

        KnownPolymorphicSerializerList globalSerializers;

        using GlobalConstruct = std::function<void(Reliquary&)>;
        using GlobalConstructList = std::vector<GlobalConstruct>;
        GlobalConstructList globalConstructList;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindGlobalStorage();
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
        RelicT* PushNewRelic(RelicT&& relic, RelicStructure additionalStructure, InitializeArgs&& ... initializeArgs);
    private:
        RelicMetadata& ValidateParentForParenting(const Handle& parent);
    private:
        template<class RelicT>
        Ptr<RelicT> CreatePtr(RelicID id) const;
    private:
        explicit ReliquaryRelics(Reliquary& owner);
        friend Reliquary;
    };
}
