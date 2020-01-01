#pragma once

#include "ReliquaryComponent.h"

#include <functional>
#include <any>

#include "SignalBatch.h"
#include "TransferableSignal.h"

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
        void ExecuteExecutionsFor(const SignalT& signal);
    public:
        class BatchSources
            : public StorageBatchSources<SignalBatchSourceBase, ReliquarySignals, BatchSources, is_signal>
        {
        public:
            template<class RelicT, std::enable_if_t<is_signal_v<RelicT>, int> = 0>
            [[nodiscard]] Map& MapFor();
            template<class RelicT, std::enable_if_t<is_signal_v<RelicT>, int> = 0>
            [[nodiscard]] const Map& MapFor() const;
        public:
            std::optional<BatchSource<TransferableSignal>> transferableSignals;

            template<class ObjectT, std::enable_if_t<is_signal<ObjectT>::value && !std::is_same_v<ObjectT, TransferableSignal>, int> = 0>
            [[nodiscard]] Arca::Batch<ObjectT> Batch() const
            {
                auto& batchSource = Required<ObjectT>();
                return Arca::Batch<ObjectT>(batchSource);
            }

            template<class ObjectT, std::enable_if_t<std::is_same_v<ObjectT, TransferableSignal>, int> = 0>
            [[nodiscard]] Arca::Batch<ObjectT> Batch() const
            {
                return Arca::Batch<ObjectT>(*const_cast<BatchSources&>(*this).transferableSignals);
            }
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
