#pragma once

#include "MatrixBatch.h"
#include "KnownMatrix.h"

namespace Arca
{
    class ReliquaryMatrices
    {
    public:
        void Clear();
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
                return found == map.end() ? nullptr : found->second.get();
            }

            template<class ObjectT, std::enable_if_t<is_object_v<ObjectT>, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>* Find() const
            {
                const auto key = KeyFor<ObjectT>();
                auto found = map.find(key);
                return found == map.end() ? nullptr : static_cast<BatchSource<ObjectT>*>(found->second.get());
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
                for (auto& stored : MatrixImplementation<ObjectT>::AllFrom(*owner->owner))
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
        Reliquary* owner;
    private:
        explicit ReliquaryMatrices(Reliquary& owner);
        ReliquaryMatrices(ReliquaryMatrices&& arg) noexcept = default;
        friend Reliquary;
    };

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void ReliquaryMatrices::EnsureInteraction(void(KnownMatrix::* interaction)(bool))
    {
        auto known = EnsuredKnown<MatrixT>();
        ((known->value).*interaction)(true);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void ReliquaryMatrices::EnsureStopInteraction(void(KnownMatrix::* stopInteraction)(bool))
    {
        auto known = EnsuredKnown<MatrixT>();
        ((known->value).*stopInteraction)(false);

        if (!known->value.IsInteractingWithAnything())
            knownList.erase(known);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    auto ReliquaryMatrices::EnsuredKnown() -> KnownList::iterator
    {
        const auto type = std::type_index(typeid(MatrixT));

        auto found = std::find_if(
            knownList.begin(),
            knownList.end(),
            [type](const StoredKnown& stored)
            {
                return type == stored.type;
            });
        if (found != knownList.end())
            return found;

        knownList.push_back(
            StoredKnown
            {
                KnownMatrix(Chroma::TypeIdentity<MatrixT>{}),
                type
            });
        return --knownList.end();
    }

    template<class T>
    [[nodiscard]] TypeName ReliquaryMatrices::BatchSources::KeyFor()
    {
        return TypeFor<T>().name;
    }
}