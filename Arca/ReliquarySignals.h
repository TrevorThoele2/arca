#pragma once

#include "ReliquaryComponent.h"

#include <functional>
#include <any>

#include "SignalBatch.h"

namespace Arca
{
    class ReliquarySignals : public ReliquaryComponent
    {
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(const SignalT& signal);
        template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(Args&& ... args);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void ExecuteOn(const std::function<void(const SignalT&)>& function);
    public:
        template<class SignalT>
        using Execution = std::function<void(const SignalT&)>;
        template<class SignalT>
        using ExecutionList = std::vector<Execution<SignalT>>;
        using ExecutionMap = std::unordered_map<TypeName, std::any>;
        ExecutionMap executionMap;

        template<class SignalT>
        void ExecuteAllFor(const SignalT& signal);
    public:
        class BatchSources
            : public StorageBatchSources<SignalBatchSourceBase, ReliquarySignals, BatchSources, is_signal>
        {
        public:
            template<class RelicT, std::enable_if_t<is_signal_v<RelicT>, int> = 0>
            [[nodiscard]] Map& MapFor();
            template<class RelicT, std::enable_if_t<is_signal_v<RelicT>, int> = 0>
            [[nodiscard]] const Map& MapFor() const;
        private:
            explicit BatchSources(ReliquarySignals& owner);
            friend ReliquarySignals;
        } batchSources = BatchSources(*this);
    public:
        ReliquarySignals(const ReliquarySignals& arg) = delete;
        ReliquarySignals& operator=(const ReliquarySignals& arg) = delete;
    private:
        explicit ReliquarySignals(Reliquary& owner);
        ReliquarySignals(ReliquarySignals&& arg) noexcept = default;
        friend Reliquary;
    };
}
