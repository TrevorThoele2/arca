#pragma once

#include "ReliquaryComponent.h"

#include <vector>
#include <any>

#include "OpenRelic.h"
#include "RelicStructure.h"
#include "RelicTraits.h"
#include "RelicMetadata.h"
#include "RelicBatchSource.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class ReliquaryRelics : public ReliquaryComponent
    {
    public:
        struct RelicPrototype
        {
            RelicID id;
            RelicOpenness openness;
        };
    public:
        using RelicMetadataList = std::vector<RelicMetadata>;
        RelicMetadataList metadataList;

        RelicID nextRelicID = 1;

        void SetupNewInternals(
            RelicID id,
            RelicOpenness openness,
            std::optional<TypeHandle> typeHandle = {},
            void* storage = nullptr);
        void DestroyMetadata(RelicID id);
        [[nodiscard]] RelicMetadata* MetadataFor(RelicID id);
        [[nodiscard]] const RelicMetadata* MetadataFor(RelicID id) const;

        void SatisfyStructure(RelicID id, const RelicStructure& structure);

        [[nodiscard]] bool WillDestroy(RelicMetadata* metadata) const;
        void Destroy(RelicMetadata& metadata);

        [[nodiscard]] RelicID NextID() const;
        [[nodiscard]] RelicID AdvanceID();

        [[nodiscard]] bool CanModify(RelicID id) const;
        void ModificationRequired(RelicID id) const;

        void ThrowIfCannotParent(const Handle& parent, RelicPrototype child);
        void Parent(const Handle& parent, const Handle& child);

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindStorage(RelicID id);
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
        using GlobalMap = std::unordered_map<TypeHandleName, std::any>;
        GlobalMap globalMap;

        KnownPolymorphicSerializerList globalSerializers;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindGlobalStorage(RelicID id);
    private:
        RelicMetadata& ValidateParentForParenting(const Handle& parent);
    private:
        explicit ReliquaryRelics(Reliquary& owner);
        friend Reliquary;
    };
}
