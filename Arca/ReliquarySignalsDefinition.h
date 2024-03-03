#pragma once

#include "ReliquarySignals.h"
#include "Reliquary.h"

namespace Arca
{
    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void ReliquarySignals::Raise(const SignalT& signal)
    {
        auto batchSource = batchSources.Find<SignalT>();
        if (!batchSource)
        {
            const auto type = TypeFor<SignalT>();
            throw NotRegistered(type, typeid(SignalT));
        }

        batchSource->Raise(signal);
        ExecuteExecutionsFor(signal);

        const TransferableSignal transferableSignal(signal);
        batchSources.transferableSignals->Raise(transferableSignal);
        ExecuteExecutionsFor(transferableSignal);
    }

    template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int>>
    void ReliquarySignals::Raise(Args&& ... args)
    {
        Raise(SignalT{ std::forward<Args>(args)... });
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void ReliquarySignals::ExecuteOn(const std::function<void(const SignalT&)>& function)
    {
        const auto typeName = TypeFor<SignalT>().name;
        auto found = executionMap.find(typeName);
        if (found == executionMap.end())
            found = executionMap.emplace(typeName, ExecutionList<SignalT>()).first;

        auto& executionList = std::any_cast<ExecutionList<SignalT>&>(found->second);
        executionList.push_back(function);
    }

    template<class SignalT>
    void ReliquarySignals::ExecuteExecutionsFor(const SignalT& signal)
    {
        const auto typeName = TypeFor<SignalT>().name;
        auto found = executionMap.find(typeName);
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

    template<class RelicT, std::enable_if_t<is_signal_v<RelicT>, int>>
    auto ReliquarySignals::BatchSources::MapFor() const -> const Map&
    {
        return map;
    }
}