#pragma once

#include <memory>
#include <unordered_set>

#include "IsMatrix.h"
#include "RelicID.h"

namespace Arca
{
    class Reliquary;

    class KnownMatrix
    {
    public:
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        explicit KnownMatrix(Chroma::TypeIdentity<MatrixT>);

        KnownMatrix(const KnownMatrix& arg);
        KnownMatrix(KnownMatrix&& arg) noexcept = default;
        KnownMatrix& operator=(const KnownMatrix& arg);
        KnownMatrix& operator=(KnownMatrix&& arg) noexcept = default;

        void Created(RelicID relicID, Reliquary& reliquary);
        void Destroying(RelicID relicID, Reliquary& reliquary);
        [[nodiscard]] bool Exists(RelicID id, Reliquary& reliquary) const;

        void InteractWithBatchSource(bool value = true);
        void InteractWithSignals(bool value = true);
        [[nodiscard]] bool IsInteractingWithAnything() const;
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;

            [[nodiscard]] virtual std::unique_ptr<Base> Clone() const = 0;

            virtual void Created(RelicID relicID, Reliquary& reliquary) = 0;
            virtual void Destroying(RelicID relicID, Reliquary& reliquary) = 0;
            [[nodiscard]] virtual bool Exists(RelicID id, Reliquary& reliquary) const = 0;

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

            void Created(RelicID relicID, Reliquary& reliquary) override;
            void Destroying(RelicID relicID, Reliquary& reliquary) override;
            [[nodiscard]] bool Exists(RelicID relicID, Reliquary& reliquary) const override;

            void InteractWithBatchSource(bool value) override;
            void InteractWithSignals(bool value) override;
            [[nodiscard]] bool IsInteractingWithAnything() const override;
        private:
            bool interactWithBatchSource = false;
            bool interactWithSignals = false;
        };

        std::unique_ptr<Base> base;
    private:
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        static void CreateBatchSourceEntry(RelicID relicID, Reliquary& reliquary);
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        static void RemoveBatchSourceEntry(RelicID relicID, Reliquary& reliquary);
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        static void SignalCreated(RelicID relicID, Reliquary& reliquary);
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        static void SignalDestroying(RelicID relicID, Reliquary& reliquary);
    };
}