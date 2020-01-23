#pragma once

#include "ReliquaryComponent.h"

#include <functional>
#include <any>

#include "SignalBatch.h"
#include "TransferableSignal.h"
#include "IsMatrixSignal.h"
#include "MatrixTypeForSignal.h"
#include "KnownMatrix.h"

namespace Arca
{
    class ReliquarySignals : public ReliquaryComponent
    {
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(const SignalT& signal);
        template<class SignalT, class... Args, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        void Raise(Args&& ... args);

        template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && !is_matrix_signal_v<SignalT>, int> = 0>
        void ExecuteOn(const std::function<void(const SignalT&)>& function);
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT> && is_matrix_signal_v<SignalT>, int> = 0>
        void ExecuteOn(const std::function<void(const SignalT&)>& function);

        void Clear();
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
        {
        public:
            using Ptr = std::unique_ptr<SignalBatchSourceBase>;
            using Map = std::unordered_map<TypeName, Ptr>;

            mutable Map map;

            template<class ObjectT, std::enable_if_t<is_signal_v<ObjectT>, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>* Find() const
            {
                const auto typeName = TypeFor<ObjectT>().name;
                auto found = map.find(typeName);
                if (found == map.end())
                    return nullptr;

                return static_cast<BatchSource<ObjectT>*>(found->second.get());
            }

            template<class ObjectT, std::enable_if_t<is_signal_v<ObjectT> && !is_matrix_signal_v<ObjectT>, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>& Required() const
            {
                return RequiredCommon<ObjectT>();
            }

            template<class ObjectT, std::enable_if_t<is_signal_v<ObjectT> && is_matrix_signal_v<ObjectT>, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>& Required() const;
        public:
            std::optional<BatchSource<TransferableSignal>> transferableSignals;

            template<class ObjectT, std::enable_if_t<is_signal_v<ObjectT> && !std::is_same_v<ObjectT, TransferableSignal>, int> = 0>
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
            template<class ObjectT>
            [[nodiscard]] BatchSource<ObjectT>& RequiredCommon() const
            {
                const auto typeName = TypeFor<ObjectT>().name;
                const auto found = Find<ObjectT>();
                if (found)
                    return *found;

                auto emplaced = map.emplace(typeName, std::make_unique<BatchSource<ObjectT>>()).first->second.get();
                return static_cast<BatchSource<ObjectT>&>(*emplaced);
            }
        private:
            ReliquarySignals* owner;
            explicit BatchSources(ReliquarySignals& owner);
            friend ReliquarySignals;
        } batchSources = BatchSources(*this);
    public:
        ReliquarySignals(const ReliquarySignals& arg) = delete;
        ReliquarySignals& operator=(const ReliquarySignals& arg) = delete;
    private:
        template<class SignalT>
        void ExecuteOnCommon(const std::function<void(const SignalT&)>& function);
    private:
        explicit ReliquarySignals(Reliquary& owner);
        ReliquarySignals(ReliquarySignals&& arg) noexcept = default;
        friend Reliquary;
    };
}
