#pragma once

#include <memory>

#include "Traits.h"

namespace Arca
{
    class Reliquary;

    class TransferableSignal
    {
    public:
        template<class T>
        explicit TransferableSignal(const T& signal);
        TransferableSignal(const TransferableSignal& arg);
        TransferableSignal(TransferableSignal&& arg) noexcept;

        void Raise(Reliquary& reliquary) const;

        [[nodiscard]] TypeName TransferringTypeName() const;
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;
            [[nodiscard]] virtual std::unique_ptr<Base> Clone() const = 0;
            virtual void Raise(Reliquary& reliquary) const = 0;
        };

        template<class T>
        class Derived final : public Base
        {
        public:
            T signal;
            explicit Derived(const T& signal);
            [[nodiscard]] std::unique_ptr<Base> Clone() const override;
            void Raise(Reliquary& reliquary) const override;
        };

        std::unique_ptr<Base> base;
        const TypeName transferringTypeName;
    };

    template<>
    struct Traits<TransferableSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeName typeName;
    };
}