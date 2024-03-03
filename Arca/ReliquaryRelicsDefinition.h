#pragma once

#include "ReliquaryRelics.h"
#include "Reliquary.h"

namespace Arca
{
    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* ReliquaryRelics::FindStorage(RelicID id)
    {
        const auto metadata = MetadataFor(id);
        if (!metadata)
            return {};

        auto& batchSource = batchSources.Required<RelicT>();
        return batchSource.Find(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    auto ReliquaryRelics::BatchSources::MapFor() -> Map&
    {
        return map;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    auto ReliquaryRelics::BatchSources::MapFor() const -> const Map&
    {
        return map;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* ReliquaryRelics::FindGlobalStorage(RelicID id)
    {
        const auto typeHandle = TypeHandleFor<RelicT>();
        const auto found = globalMap.find(typeHandle.name);
        if (found == globalMap.end())
            throw NotRegistered(typeHandle, typeid(RelicT));

        return std::any_cast<RelicT>(&found->second);
    }
}