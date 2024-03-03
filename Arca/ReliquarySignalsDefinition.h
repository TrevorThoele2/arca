#pragma once

#include "ReliquarySignals.h"
#include "Reliquary.h"

namespace Arca
{
    template<class SignalT>
    void ReliquarySignals::ExecuteAllFor(const SignalT& signal)
    {
        const auto typeHandleName = TypeHandleFor<SignalT>().name;
        auto found = executionMap.find(typeHandleName);
        if (found == executionMap.end())
            return;

        auto& executionList = std::any_cast<ExecutionList<SignalT>&>(found->second);
        for (auto& loop : executionList)
            loop(signal);
    }

    template<class RelicT, std::enable_if_t<is_signal_v<RelicT>, int>>
    auto ReliquarySignals::BatchSources::MapFor() -> Map&
    {
        return map;
    }
}