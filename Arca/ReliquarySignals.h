#pragma once

#include <functional>

#include "IsSignal.h"
#include "IsMatrixSignal.h"
#include "MatrixTypeForSignal.h"
#include "KnownMatrix.h"

namespace Arca
{
    class ReliquarySignals
    {
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(const SignalT& signal);
        template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(Args&& ... args);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && !is_matrix_signal_v<SignalT>, int> = 0>
        void On(const std::function<void(const SignalT&)>& function);
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && is_matrix_signal_v<SignalT>, int> = 0>
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
        Reliquary* owner;
    private:
        explicit ReliquarySignals(Reliquary& owner);
        ReliquarySignals(ReliquarySignals&& arg) noexcept = default;
        friend Reliquary;
    };
}
