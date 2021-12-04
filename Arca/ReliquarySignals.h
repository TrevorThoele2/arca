#pragma once

#include <functional>

#include "IsSignal.h"
#include "MatrixTypeForSignal.h"
#include "MatrixInteraction.h"

namespace Arca
{
    class ReliquarySignals
    {
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(const SignalT& signal);
        template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(Args&& ... args);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void On(const std::function<void(const SignalT&)>& function);
    public:
        template<class SignalT>
        using Listener = std::function<void(const SignalT&)>;
        class ListenerListBase
        {
        public:
            virtual ~ListenerListBase() = 0;
        };

        template<class SignalT>
        class ListenerListDerived final : public ListenerListBase
        {
        public:
            std::list<Listener<SignalT>> value;
        };

        using ListenerListPtr = std::unique_ptr<ListenerListBase>;
        using ListenerMap = std::unordered_map<TypeName, ListenerListPtr>;
        ListenerMap listenerMap;

        template<class SignalT>
        void ExecuteListenersFor(const SignalT& signal);
    public:
        ReliquarySignals(const ReliquarySignals& arg) = delete;
        ReliquarySignals& operator=(const ReliquarySignals& arg) = delete;
    private:
        template<class SignalT>
        void ExecuteOnCommon(const std::function<void(const SignalT&)>& function);
    private:
        explicit ReliquarySignals();
        ReliquarySignals(ReliquarySignals&& arg) noexcept = default;
        friend class Reliquary;
    };

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

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    void ReliquarySignals::On(const std::function<void(const SignalT&)>& function)
    {
        ExecuteOnCommon<SignalT>(function);
    }

    template<class SignalT>
    void ReliquarySignals::ExecuteListenersFor(const SignalT& signal)
    {
        const auto typeName = FullTypeNameFor<SignalT>();
        auto found = listenerMap.find(typeName);
        if (found != listenerMap.end())
        {
            using ListenerListDerivedT = ListenerListDerived<SignalT>;
            auto listenerList = dynamic_cast<ListenerListDerivedT*>(found->second.get());
            for (auto& execution : listenerList->value)
                execution(signal);
        }
    }

    template<class SignalT>
    void ReliquarySignals::ExecuteOnCommon(const std::function<void(const SignalT&)>& function)
    {
        using ListenerListDerivedT = ListenerListDerived<SignalT>;

        const auto typeName = FullTypeNameFor<SignalT>();
        auto found = listenerMap.find(typeName);
        if (found == listenerMap.end())
            found = listenerMap.emplace(typeName, std::make_unique<ListenerListDerivedT>()).first;

        auto listenerList = dynamic_cast<ListenerListDerivedT*>(found->second.get());
        listenerList->value.push_back(function);
    }
}
