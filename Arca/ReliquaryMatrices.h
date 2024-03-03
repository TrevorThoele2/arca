#pragma once

#include "MatrixBatch.h"
#include "MatrixInteraction.h"
#include "ReliquarySignals.h"
#include <Chroma/TypeIdentity.h>

namespace Arca
{
    class Reliquary;

    class ReliquaryMatrices
    {
    public:
        void Clear();
    public:
        void ShardCreated(RelicID id, const Type& type, const std::unordered_set<Type>& allTypes);
        void ShardDestroying(RelicID id, const Type& type, const std::unordered_set<Type>& allTypes);

        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        void EnsureInteraction(MatrixInteraction interaction);
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        void EnsureStopInteraction(MatrixInteraction interaction);
    public:
        ReliquaryMatrices(const ReliquaryMatrices& arg) = delete;
        ReliquaryMatrices& operator=(const ReliquaryMatrices& arg) = delete;
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
                owner->EnsureStopInteraction<ObjectT>(MatrixInteraction::BatchSource);
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
                const auto onReferenceCountZero = [this]() { owner->batchSources.Destroy<ObjectT>(TypeFor<ObjectT>().name); };
                auto emplaced = map.emplace(key, std::make_unique<BatchSource<ObjectT>>(onReferenceCountZero)).first->second.get();
                auto& batchSource = static_cast<BatchSource<ObjectT>&>(*emplaced);
                for (auto& stored : MatrixImplementation<ObjectT>::AllFrom(*owner->owner))
                    batchSource.Add(stored);
                owner->EnsureInteraction<ObjectT>(MatrixInteraction::BatchSource);
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
        class Stored
        {
        public:
            template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
            explicit Stored(Chroma::TypeIdentity<MatrixT>);

            Stored(const Stored& arg);
            Stored(Stored&& arg) noexcept = default;
            Stored& operator=(const Stored& arg);
            Stored& operator=(Stored&& arg) noexcept = default;

            void Created(
                RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals);
            void Destroying(
                RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals);
            [[nodiscard]] bool Exists(const std::unordered_set<Type>& shards) const;

            [[nodiscard]] bool Contains(Type type) const;

            void InteractWithBatchSource(bool value = true);
            void InteractWithSignals(bool value = true);
            [[nodiscard]] bool IsInteractingWithAnything() const;

            [[nodiscard]] std::type_index MatrixType() const;
        private:
            std::type_index matrixType;
        private:
            class Base
            {
            public:
                virtual ~Base() = 0;

                [[nodiscard]] virtual std::unique_ptr<Base> Clone() const = 0;

                virtual void Created(
                    RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals) = 0;
                virtual void Destroying(
                    RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals) = 0;
                [[nodiscard]] virtual bool Exists(const std::unordered_set<Type>& shards) const = 0;

                [[nodiscard]] virtual bool Contains(Type type) const = 0;

                virtual void InteractWithBatchSource(bool value) = 0;
                virtual void InteractWithSignals(bool value) = 0;
                [[nodiscard]] virtual bool IsInteractingWithAnything() const = 0;
            };

            template<class MatrixT>
            class Derived final : public Base
            {
            public:
                Derived() = default;

                [[nodiscard]] std::unique_ptr<Base> Clone() const override;

                void Created(
                    RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals) override;
                void Destroying(
                    RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals) override;
                [[nodiscard]] bool Exists(const std::unordered_set<Type>& shards) const override;

                [[nodiscard]] bool Contains(Type type) const override;

                void InteractWithBatchSource(bool value) override;
                void InteractWithSignals(bool value) override;
                [[nodiscard]] bool IsInteractingWithAnything() const override;
            private:
                bool interactWithBatchSource = false;
                bool interactWithSignals = false;
            };

            std::unique_ptr<Base> base;
        };

        using StoredList = std::vector<Stored>;
        StoredList storedList;

        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        StoredList::iterator RequiredStored();
    private:
        Reliquary* owner;
        ReliquarySignals* signals;
    private:
        explicit ReliquaryMatrices(Reliquary& owner, ReliquarySignals& signals);
        ReliquaryMatrices(ReliquaryMatrices&& arg) noexcept = default;
        friend Reliquary;
    };

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void ReliquaryMatrices::EnsureInteraction(MatrixInteraction interaction)
    {
        auto stored = RequiredStored<MatrixT>();
        switch (interaction)
        {
        case MatrixInteraction::BatchSource:
            stored->InteractWithBatchSource(true);
        case MatrixInteraction::Signal:
            stored->InteractWithSignals(true);
        }
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void ReliquaryMatrices::EnsureStopInteraction(MatrixInteraction interaction)
    {
        auto stored = RequiredStored<MatrixT>();
        switch (interaction)
        {
        case MatrixInteraction::BatchSource:
            stored->InteractWithBatchSource(false);
        case MatrixInteraction::Signal:
            stored->InteractWithSignals(false);
        }

        if (!stored->IsInteractingWithAnything())
            storedList.erase(stored);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    auto ReliquaryMatrices::RequiredStored() -> StoredList::iterator
    {
        const auto type = std::type_index(typeid(MatrixT));

        auto found = std::find_if(
            storedList.begin(),
            storedList.end(),
            [type](const Stored& stored)
            {
                return type == stored.MatrixType();
            });
        if (found != storedList.end())
            return found;

        storedList.push_back(Stored { Chroma::TypeIdentity<MatrixT>{} });
        return --storedList.end();
    }

    template<class T>
    [[nodiscard]] TypeName ReliquaryMatrices::BatchSources::KeyFor()
    {
        return TypeFor<T>().name;
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    ReliquaryMatrices::Stored::Stored(Chroma::TypeIdentity<MatrixT>) :
        base(std::make_unique<Derived<MatrixT>>()), matrixType(typeid(MatrixT))
    {}

    template<class MatrixT>
    auto ReliquaryMatrices::Stored::Derived<MatrixT>::Clone() const -> std::unique_ptr<Base>
    {
        return std::make_unique<Derived>(*this);
    }

    template<class MatrixT>
    void ReliquaryMatrices::Stored::Derived<MatrixT>::Created(
        RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals)
    {
        if (interactWithBatchSource)
        {
            auto& batchSource = batchSources.Required<MatrixT>();
            if (!batchSource.Has(relicID))
                batchSource.Add({ relicID, MatrixImplementation<MatrixT>::CreateStored(relicID, reliquary) });
        }

        if (interactWithSignals)
        {
            Index<MatrixT> index{ relicID, reliquary, MatrixImplementation<MatrixT>::CreateIndexValue(relicID, reliquary) };
            signals.Raise(MatrixFormed<MatrixT>{index});
        }
    }

    template<class MatrixT>
    void ReliquaryMatrices::Stored::Derived<MatrixT>::Destroying(
        RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals)
    {
        if (interactWithBatchSource)
        {
            auto& batchSource = batchSources.Required<MatrixT>();
            batchSource.Remove(relicID);
        }

        if (interactWithSignals)
        {
            Index<MatrixT> index{ relicID, reliquary, MatrixImplementation<MatrixT>::CreateIndexValue(relicID, reliquary) };
            signals.Raise(MatrixDissolved<MatrixT>{index});
        }
    }

    template<class MatrixT>
    bool ReliquaryMatrices::Stored::Derived<MatrixT>::Exists(const std::unordered_set<Type>& shards) const
    {
        return MatrixImplementation<MatrixT>::IsIn(shards);
    }

    template<class MatrixT>
    bool ReliquaryMatrices::Stored::Derived<MatrixT>::Contains(Type type) const
    {
        return MatrixContains<MatrixT>::Contains(type);
    }

    template<class MatrixT>
    void ReliquaryMatrices::Stored::Derived<MatrixT>::InteractWithBatchSource(bool value)
    {
        interactWithBatchSource = value;
    }

    template<class MatrixT>
    void ReliquaryMatrices::Stored::Derived<MatrixT>::InteractWithSignals(bool value)
    {
        interactWithSignals = value;
    }

    template<class MatrixT>
    bool ReliquaryMatrices::Stored::Derived<MatrixT>::IsInteractingWithAnything() const
    {
        return interactWithBatchSource || interactWithSignals;
    }
}