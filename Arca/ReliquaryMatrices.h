#pragma once

#include "ReliquaryComponent.h"

#include "MatrixBatch.h"
#include "KnownMatrix.h"

#include "EitherImplementation.h"
#include "AllImplementation.h"

namespace Arca
{
    class ReliquaryMatrices : public ReliquaryComponent
    {
    public:
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        void Destroy(RelicID id);
        void Clear();

        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;
    public:
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        void EnsureInteraction(void(KnownMatrix::* interaction)(bool));
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        void EnsureStopInteraction(void(KnownMatrix::* stopInteraction)(bool));
    public:
        ReliquaryMatrices(const ReliquaryMatrices& arg) = delete;
        ReliquaryMatrices& operator=(const ReliquaryMatrices& arg) = delete;
    public:
        class CreationTransaction
        {
        public:
            CreationTransaction(const CreationTransaction& arg) = default;
            CreationTransaction(CreationTransaction&& arg) noexcept = default;

            void Finalize();
        private:
            explicit CreationTransaction(std::vector<KnownMatrix*>&& knownMatrices, RelicID id, Reliquary& owner);
            RelicID id;
            Reliquary* owner;
            std::vector<KnownMatrix*> knownMatrices;
            friend ReliquaryMatrices;
        };

        class DestroyingTransaction
        {
        public:
            DestroyingTransaction(const DestroyingTransaction& arg) = default;
            DestroyingTransaction(DestroyingTransaction&& arg) noexcept = default;

            void Finalize(Type type);
        private:
            explicit DestroyingTransaction(std::vector<KnownMatrix*>&& knownMatrices, RelicID id, Reliquary& owner);
            RelicID id;
            Reliquary* owner;
            std::vector<KnownMatrix*> knownMatrices;
            friend ReliquaryMatrices;
        };

        CreationTransaction StartCreationTransaction(RelicID id);
        DestroyingTransaction StartDestroyingTransaction(RelicID id);
    public:
        class BatchSources
        {
        public:
            using Ptr = std::unique_ptr<MatrixBatchSourceBase>;
            using Map = std::unordered_map<TypeName, Ptr>;

            mutable Map map;

            template<class ObjectT>
            static constexpr bool is_object_v = is_matrix_v<ObjectT>;

            template<class ObjectT, std::enable_if_t<is_object_v<ObjectT>, int> = 0>
            void Destroy(const TypeName& key)
            {
                map.erase(key);
                owner->EnsureStopInteraction<ObjectT>(&KnownMatrix::InteractWithBatchSource);
            }

            void Clear()
            {
                map.clear();
            }

            [[nodiscard]] MatrixBatchSourceBase* Find(const TypeName& key) const
            {
                const auto found = map.find(key);
                if (found == map.end())
                    return nullptr;

                return found->second.get();
            }

            template<class ObjectT, std::enable_if_t<is_object_v<ObjectT>, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>* Find() const
            {
                const auto key = KeyFor<ObjectT>();
                auto found = map.find(key);
                if (found == map.end())
                    return nullptr;

                return static_cast<BatchSource<ObjectT>*>(found->second.get());
            }

            template<class ObjectT, std::enable_if_t<is_object_v<ObjectT>, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>& Required() const
            {
                auto found = Find<ObjectT>();
                if (found)
                    return *found;

                const auto key = KeyFor<ObjectT>();
                auto emplaced = map.emplace(key, std::make_unique<BatchSource<ObjectT>>(*owner)).first->second.get();
                auto& batchSource = static_cast<BatchSource<ObjectT>&>(*emplaced);
                for (auto& stored : MatrixImplementation<ObjectT>::AllFrom(owner->Owner()))
                    batchSource.Add(stored);
                owner->EnsureInteraction<ObjectT>(&KnownMatrix::InteractWithBatchSource);
                return batchSource;
            }

            template<class ObjectT, std::enable_if_t<is_object_v<ObjectT>, int> = 0>
            [[nodiscard]] Arca::Batch<ObjectT> Batch() const
            {
                auto& batchSource = Required<ObjectT>();
                return Arca::Batch<ObjectT>(batchSource);
            }
        private:
            ReliquaryMatrices* owner;
        private:
            explicit BatchSources(ReliquaryMatrices& owner);
            friend ReliquaryMatrices;

            template<class T>
            [[nodiscard]] static TypeName KeyFor();
        } batchSources = BatchSources(*this);
    private:
        struct StoredKnown
        {
            KnownMatrix value;
            std::type_index type;
        };

        using KnownList = std::vector<StoredKnown>;
        KnownList knownList;

        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        KnownList::iterator EnsuredKnown();
    private:
        explicit ReliquaryMatrices(Reliquary& owner);
        ReliquaryMatrices(ReliquaryMatrices&& arg) noexcept = default;
        friend Reliquary;
    };
}