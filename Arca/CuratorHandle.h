#pragma once

#include <typeindex>
#include <memory>

#include "Curator.h"
#include "CuratorTypeDescription.h"

namespace Arca
{
    class CuratorHandle
    {
    public:
        const CuratorTypeDescription description;
    public:
        explicit CuratorHandle(CuratorTypeDescription description);
        CuratorHandle(const CuratorHandle& arg) = default;
        CuratorHandle(CuratorHandle&& arg) = default;

        virtual ~CuratorHandle() = 0;

        CuratorHandle& operator=(const CuratorHandle& arg);
        CuratorHandle& operator=(CuratorHandle&& arg) noexcept;

        [[nodiscard]] virtual Curator* Get() = 0;
        [[nodiscard]] virtual std::type_index Type() = 0;
        [[nodiscard]] virtual bool IsOwning() const = 0;
    };

    class OwnedCuratorHandle final : public CuratorHandle
    {
    public:
        explicit OwnedCuratorHandle(std::unique_ptr<Curator>&& ptr, CuratorTypeDescription description);
        [[nodiscard]] Curator* Get() override;
        [[nodiscard]] std::type_index Type() override;
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
        explicit UnownedCuratorHandle(Curator* ptr, CuratorTypeDescription description);
        [[nodiscard]] Curator* Get() override;
        [[nodiscard]] std::type_index Type() override;
        [[nodiscard]] bool IsOwning() const override;
    private:
        Curator* ptr;
        const std::type_index type;
    private:
        INSCRIPTION_ACCESS;
    };
}