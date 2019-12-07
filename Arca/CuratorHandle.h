#pragma once

#include <typeindex>
#include <memory>

#include "Curator.h"
#include "Type.h"

namespace Arca
{
    class CuratorHandle
    {
    public:
        const Type type;
    public:
        explicit CuratorHandle(Type type);
        CuratorHandle(const CuratorHandle& arg) = default;
        CuratorHandle(CuratorHandle&& arg) = default;

        virtual ~CuratorHandle() = 0;

        CuratorHandle& operator=(const CuratorHandle& arg);
        CuratorHandle& operator=(CuratorHandle&& arg) noexcept;

        [[nodiscard]] virtual Curator* Get() = 0;
        [[nodiscard]] virtual std::type_index TypeIndex() = 0;
        [[nodiscard]] virtual bool IsOwning() const = 0;
    };

    class OwnedCuratorHandle final : public CuratorHandle
    {
    public:
        explicit OwnedCuratorHandle(std::unique_ptr<Curator>&& ptr, Arca::Type type);
        [[nodiscard]] Curator* Get() override;
        [[nodiscard]] std::type_index TypeIndex() override;
        [[nodiscard]] bool IsOwning() const override;
    private:
        const std::unique_ptr<Curator> ptr;
        const std::type_index type;
    private:
        INSCRIPTION_ACCESS;
    };

    class UnownedCuratorHandle final : public CuratorHandle
    {
    public:
        explicit UnownedCuratorHandle(Curator* ptr, Type type);
        [[nodiscard]] Curator* Get() override;
        [[nodiscard]] std::type_index TypeIndex() override;
        [[nodiscard]] bool IsOwning() const override;
    private:
        Curator* ptr;
        const std::type_index type;
    private:
        INSCRIPTION_ACCESS;
    };
}
