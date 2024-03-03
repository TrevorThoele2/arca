#pragma once

#include <optional>

namespace Arca
{
    class Reliquary;

    class RelicFactoryBase
    {
    public:
        explicit RelicFactoryBase(Reliquary& owner);
        RelicFactoryBase(const RelicFactoryBase& arg) = default;
        RelicFactoryBase(RelicFactoryBase&& arg) = default;

        virtual ~RelicFactoryBase() = 0;

        RelicFactoryBase& operator=(const RelicFactoryBase& arg) = delete;
        RelicFactoryBase& operator=(RelicFactoryBase&& arg) = delete;
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
    private:
        Reliquary* owner;
        friend Reliquary;
    };
}