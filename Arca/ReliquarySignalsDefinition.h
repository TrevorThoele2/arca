#pragma once

#include "ReliquarySignals.h"
#include "Reliquary.h"

namespace Arca
{
    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void ReliquarySignals::Raise(const SignalT& signal)
    {
        ExecuteListenersFor(signal);
    }

    template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int>>
    void ReliquarySignals::Raise(Args&& ... args)
    {
        Raise(SignalT{ std::forward<Args>(args)... });
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && !is_matrix_signal_v<SignalT>, int>>
    void ReliquarySignals::On(const std::function<void(const SignalT&)>& function)
    {
        ExecuteOnCommon<SignalT>(function);
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && is_matrix_signal_v<SignalT>, int>>
    void ReliquarySignals::On(const std::function<void(const SignalT&)>& function)
    {
        ExecuteOnCommon<SignalT>(function);
        Owner().matrices.EnsureInteraction<typename MatrixTypeForSignal<SignalT>::Type>
            (&KnownMatrix::InteractWithSignals);
    }

    template<class SignalT>
    void ReliquarySignals::ExecuteListenersFor(const SignalT& signal)
    {
        const auto typeName = TypeFor<SignalT>().name;
        auto found = listenerMap.find(typeName);
        if (found == listenerMap.end())
            return;

        auto& executionList = std::any_cast<ListenerList<SignalT>&>(found->second);
        for (auto& loop : executionList)
            loop(signal);
    }

    template<class SignalT>
    void ReliquarySignals::ExecuteOnCommon(const std::function<void(const SignalT&)>& function)
    {
        const auto typeName = TypeFor<SignalT>().name;
        auto found = listenerMap.find(typeName);
        if (found == listenerMap.end())
            found = listenerMap.emplace(typeName, ListenerList<SignalT>()).first;

        auto& executionList = std::any_cast<ListenerList<SignalT>&>(found->second);
        executionList.push_back(function);
    }
}