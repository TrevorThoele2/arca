#pragma once

#include "ReliquaryComponent.h"

#include <vector>
#include <any>

#include "DynamicRelic.h"
#include "RelicStructure.h"
#include "RelicTraits.h"
#include "RelicMetadata.h"
#include "RelicBatchSource.h"

#include "IntervalList.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class ReliquaryRelics : public ReliquaryComponent
    {
    public:
        using RelicMetadataList = std::vector<RelicMetadata>;
        RelicMetadataList metadataList;

        IntervalList<RelicID> occupiedIDs;

        void SetupNewInternals(
            RelicID id,
            RelicDynamism dynamism,
            std::optional<TypeHandle> typeHandle = {},
            void* storage = nullptr);
        void DestroyMetadata(RelicID id);
        [[nodiscard]] RelicMetadata* MetadataFor(RelicID id);
        [[nodiscard]] const RelicMetadata* MetadataFor(RelicID id) const;

        void SatisfyStructure(RelicID id, const RelicStructure& structure);

        [[nodiscard]] bool WillDestroy(RelicMetadata* metadata) const;
        void Destroy(RelicMetadata& metadata);

        [[nodiscard]] RelicID NextID() const;

        [[nodiscard]] bool CanModify(RelicID id) const;
        void ModificationRequired(RelicID id) const;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindStorage(RelicID id);
    public:
        class BatchSources : public BatchSourcesBase<RelicBatchSourceBase, ReliquaryRelics, BatchSources>
        {
        private:
            template<class RelicT>
            constexpr static bool is_object_v = is_relic_v<RelicT>;
            friend BatchSourcesBase<RelicBatchSourceBase, ReliquaryRelics, BatchSources>;
        public:
            template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
            [[nodiscard]] Map& MapFor();
        private:
            explicit BatchSources(ReliquaryRelics& owner);
            friend ReliquaryRelics;
        } batchSources = BatchSources(*this);

        KnownPolymorphicSerializerList serializers;
    public:
        using StaticMap = std::unordered_map<TypeHandleName, std::any>;
        StaticMap staticMap;

        KnownPolymorphicSerializerList staticSerializers;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindStaticStorage(RelicID id);
    private:
        explicit ReliquaryRelics(Reliquary& owner);
        friend Reliquary;
    };
}
