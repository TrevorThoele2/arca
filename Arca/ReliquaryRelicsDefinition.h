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

        auto& batchSource = RequiredBatchSource<RelicT>();
        return batchSource.Find(id);
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    BatchSource<RelicT>* ReliquaryRelics::FindBatchSource()
    {
        const auto typeHandleName = TypeHandleFor<RelicT>().name;
        auto found = batchSources.find(typeHandleName);
        if (found == batchSources.end())
            return nullptr;

        return static_cast<BatchSource<RelicT>*>(found->second.get());
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    BatchSource<RelicT>& ReliquaryRelics::RequiredBatchSource()
    {
        auto found = FindBatchSource<RelicT>();
        if (!found)
        {
            const auto typeHandle = TypeHandleFor<RelicT>();
            throw NotRegistered(typeHandle, typeid(RelicT));
        }

        return *found;
    }

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* ReliquaryRelics::FindStaticStorage(RelicID id)
    {
        const auto typeHandle = TypeHandleFor<RelicT>();
        const auto found = staticMap.find(typeHandle.name);
        if (found == staticMap.end())
            throw NotRegistered(typeHandle, typeid(RelicT));

        return std::any_cast<RelicT>(&found->second);
    }
}