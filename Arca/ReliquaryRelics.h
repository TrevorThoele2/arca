#pragma once

#include "ReliquaryComponent.h"

#include <vector>
#include <any>

#include "OpenRelic.h"
#include "RelicStructure.h"
#include "RelicTraits.h"
#include "RelicMetadata.h"
#include "RelicBatchSource.h"
#include "HasFactoryMethod.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class ReliquaryRelics : public ReliquaryComponent
    {
    public:
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> Create(CreationArgs&& ... creationArgs);
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateWith(const RelicStructure& structure, CreationArgs&& ... creationArgs);
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateWith(const std::string& structureName, CreationArgs&& ... creationArgs);

        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateChild(const Handle& parent, CreationArgs&& ... creationArgs);
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateChildWith(const Handle& parent, const RelicStructure& structure, CreationArgs&& ... creationArgs);
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        Ptr<RelicT> CreateChildWith(const Handle& parent, const std::string& structureName, CreationArgs&& ... creationArgs);

        template<class RelicT, std::enable_if_t<is_local_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Ptr<RelicT> Find(RelicID id) const;
        template<class RelicT, std::enable_if_t<is_global_relic_v<RelicT>, int> = 0>
        [[nodiscard]] Ptr<RelicT> Find() const;
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
            TypeHandle typeHandle = {},
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
        using Initializer = void(*)(Reliquary&);
        using InitializerList = std::vector<Initializer>;
        InitializerList initializers;
    public:
        class BatchSources : public StorageBatchSources<RelicBatchSourceBase, ReliquaryRelics, BatchSources>
        { 
        public:
            template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
            [[nodiscard]] Map& MapFor();
            template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
            [[nodiscard]] const Map& MapFor() const;
        private:
            explicit BatchSources(ReliquaryRelics& owner);
            friend ReliquaryRelics;
        private:
            template<class RelicT>
            constexpr static bool is_object_v = is_relic_v<RelicT>;
            friend StorageBatchSources<RelicBatchSourceBase, ReliquaryRelics, BatchSources>;
        } batchSources = BatchSources(*this);

        KnownPolymorphicSerializerList serializers;
    public:
        struct StoredGlobal
        {
            std::shared_ptr<void> storage;
            RelicID id;
        };

        using GlobalMap = std::unordered_map<TypeHandleName, StoredGlobal>;
        GlobalMap globalMap;

        KnownPolymorphicSerializerList globalSerializers;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindGlobalStorage();
    private:
        template<
            class RelicT,
            class... CreationArgs,
            std::enable_if_t<is_relic_v<RelicT> && has_factory_method_v<RelicT>, int> = 0>
        std::optional<RelicT> CreateRelic(CreationArgs&& ... creationArgs);
        template<
            class RelicT,
            class... CreationArgs,
            std::enable_if_t<is_relic_v<RelicT> && !has_factory_method_v<RelicT>, int> = 0>
        std::optional<RelicT> CreateRelic(CreationArgs&& ... creationArgs);
        template<class RelicT>
        Ptr<RelicT> PushNewRelic(RelicT&& relic, RelicStructure additionalStructure);
    private:
        RelicMetadata& ValidateParentForParenting(const Handle& parent);
    private:
        explicit ReliquaryRelics(Reliquary& owner);
        friend Reliquary;
    };
}
