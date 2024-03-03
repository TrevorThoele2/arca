#pragma once

#include "ReliquaryComponent.h"

#include <vector>
#include <any>

#include "DynamicRelic.h"
#include "TypedRelic.h"
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
        explicit ReliquaryRelics(Reliquary& owner);
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
        using BatchSourcePtr = std::unique_ptr<RelicBatchSourceBase>;
        using BatchSourceMap = std::unordered_map<TypeHandleName, BatchSourcePtr>;
        BatchSourceMap batchSources;

        KnownPolymorphicSerializerList serializers;

        [[nodiscard]] RelicBatchSourceBase* FindBatchSource(const TypeHandleName& typeHandle);
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] BatchSource<RelicT>* FindBatchSource();
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        [[nodiscard]] BatchSource<RelicT>& RequiredBatchSource();
    public:
        using StaticMap = std::unordered_map<TypeHandleName, std::any>;
        StaticMap staticMap;

        KnownPolymorphicSerializerList staticSerializers;

        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* FindStaticStorage(RelicID id);
    };
}
