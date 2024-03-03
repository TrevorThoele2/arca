#pragma once

#include "ReliquaryComponent.h"

#include <functional>
#include <any>

#include "SignalBatchSource.h"

namespace Arca
{
    class ReliquarySignals : public ReliquaryComponent
    {
    public:
        template<class SignalT>
        using Execution = std::function<void(const SignalT&)>;
        template<class SignalT>
        using ExecutionList = std::vector<Execution<SignalT>>;
        using ExecutionMap = std::unordered_map<TypeHandleName, std::any>;
        ExecutionMap executionMap;

        template<class SignalT>
        void ExecuteAllFor(const SignalT& signal);
    public:
        class BatchSources : public BatchSourcesBase<SignalBatchSourceBase, ReliquarySignals, BatchSources>
        {
        private:
            template<class RelicT>
            constexpr static bool is_object_v = is_signal_v<RelicT>;
            friend BatchSourcesBase<SignalBatchSourceBase, ReliquarySignals, BatchSources>;
        public:
            template<class RelicT, std::enable_if_t<is_signal_v<RelicT>, int> = 0>
            [[nodiscard]] Map& MapFor();
            template<class RelicT, std::enable_if_t<is_signal_v<RelicT>, int> = 0>
            [[nodiscard]] const Map& MapFor() const;
        private:
            explicit BatchSources(ReliquarySignals& owner);
            friend ReliquarySignals;
        } batchSources = BatchSources(*this);
    private:
        explicit ReliquarySignals(Reliquary& owner);
        friend Reliquary;
    };
}
