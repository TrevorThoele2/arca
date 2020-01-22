#pragma once

#include "ReliquarySignals.h"
#include "Reliquary.h"

namespace Arca
{
    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void ReliquarySignals::Raise(const SignalT& signal)
    {
        auto& batchSource = batchSources.Required<SignalT>();

        batchSource.Raise(signal);
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

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && !is_matrix_signal_v<SignalT>, int>>
    void ReliquarySignals::ExecuteOn(const std::function<void(const SignalT&)>& function)
    {
        ExecuteOnCommon<SignalT>(function);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && is_matrix_signal_v<SignalT>, int>>
    void ReliquarySignals::ExecuteOn(const std::function<void(const SignalT&)>& function)
    {
        ExecuteOnCommon<SignalT>(function);
        Owner().matrices.EnsureInteraction<typename MatrixTypeForSignal<SignalT>::Type>
            (&KnownMatrix::InteractWithSignals);
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

    template<class ObjectT, std::enable_if_t<is_signal_v<ObjectT> && is_matrix_signal_v<ObjectT>, int>>
    BatchSource<ObjectT>& ReliquarySignals::BatchSources::Required() const
    {
        auto& batchSource = RequiredCommon<ObjectT>();
        owner->Owner().matrices.EnsureInteraction<typename MatrixTypeForSignal<ObjectT>::Type>
            (&KnownMatrix::InteractWithSignals);
        return batchSource;
    }

    template<class SignalT>
    void ReliquarySignals::ExecuteOnCommon(const std::function<void(const SignalT&)>& function)
    {
        const auto typeName = TypeFor<SignalT>().name;
        auto found = executionMap.find(typeName);
        if (found == executionMap.end())
            found = executionMap.emplace(typeName, ExecutionList<SignalT>()).first;

        auto& executionList = std::any_cast<ExecutionList<SignalT>&>(found->second);
        executionList.push_back(function);
    }
}