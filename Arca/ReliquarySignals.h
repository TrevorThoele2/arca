#pragma once

#include "ReliquaryComponent.h"

#include <functional>
#include <typeindex>
#include <any>

#include "SignalBatchSource.h"

namespace Arca
{
    class ReliquarySignals : public ReliquaryComponent
    {
    public:
        explicit ReliquarySignals(Reliquary& owner);
    public:
        template<class SignalT>
        using Execution = std::function<void(const SignalT&)>;
        template<class SignalT>
        using ExecutionList = std::vector<Execution<SignalT>>;
        using ExecutionMap = std::unordered_map<std::type_index, std::any>;
        ExecutionMap executionMap;

        template<class SignalT>
        void ExecuteAllFor(const SignalT& signal);
    public:
        using BatchSourcePtr = std::unique_ptr<SignalBatchSourceBase>;
        using BatchSourceList = std::unordered_map<std::type_index, BatchSourcePtr>;
        BatchSourceList batchSources;

        [[nodiscard]] SignalBatchSourceBase* FindBatchSource(const std::type_index& type);
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        [[nodiscard]] BatchSource<SignalT>* FindBatchSource();
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        [[nodiscard]] const BatchSource<SignalT>* FindBatchSource() const;
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        [[nodiscard]] BatchSource<SignalT>& RequiredBatchSource();

        template<class SignalT>
        [[nodiscard]] static std::type_index KeyForBatchSource();
    };
}
