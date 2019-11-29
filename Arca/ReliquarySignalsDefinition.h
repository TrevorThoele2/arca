#pragma once

#include "ReliquarySignals.h"
#include "Reliquary.h"

namespace Arca
{
    template<class SignalT>
    void ReliquarySignals::ExecuteAllFor(const SignalT& signal)
    {
        const auto typeIndex = std::type_index(typeid(SignalT));
        auto found = executionMap.find(typeIndex);
        if (found == executionMap.end())
            return;

        auto& executionList = std::any_cast<ExecutionList<SignalT>&>(found->second);
        for (auto& loop : executionList)
            loop(signal);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    BatchSource<SignalT>* ReliquarySignals::FindBatchSource()
    {
        auto found = batchSources.find(KeyForBatchSource<SignalT>());
        if (found == batchSources.end())
            return nullptr;

        return static_cast<BatchSource<SignalT>*>(found->second.get());
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    const BatchSource<SignalT>* ReliquarySignals::FindBatchSource() const
    {
        return const_cast<ReliquarySignals*>(this)->FindBatchSource<SignalT>();
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    BatchSource<SignalT>& ReliquarySignals::RequiredBatchSource()
    {
        auto found = FindBatchSource<SignalT>();
        if (!found)
            throw NotRegistered({ typeid(SignalT).name(), std::is_const_v<SignalT> });

        return *found;
    }

    template<class SignalT>
    std::type_index ReliquarySignals::KeyForBatchSource()
    {
        return typeid(SignalT);
    }
}