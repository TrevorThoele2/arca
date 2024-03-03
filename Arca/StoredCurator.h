#pragma once

#include <typeindex>
#include <memory>

#include "Curator.h"
#include "Type.h"

namespace Arca
{
    class StoredCurator
    {
    public:
        const Type type;
    public:
        explicit StoredCurator(Type type);
        StoredCurator(const StoredCurator& arg) = default;
        StoredCurator(StoredCurator&& arg) = default;

        virtual ~StoredCurator() = 0;

        StoredCurator& operator=(const StoredCurator& arg);
        StoredCurator& operator=(StoredCurator&& arg) noexcept;

        [[nodiscard]] virtual Curator* Get() = 0;
        [[nodiscard]] virtual std::type_index TypeIndex() = 0;
        [[nodiscard]] virtual bool IsOwning() const = 0;
    };

    class OwnedStoredCurator final : public StoredCurator
    {
    public:
        explicit OwnedStoredCurator(std::unique_ptr<Curator>&& ptr, Arca::Type type);
        [[nodiscard]] Curator* Get() override;
        [[nodiscard]] std::type_index TypeIndex() override;
        [[nodiscard]] bool IsOwning() const override;
    private:
        const std::unique_ptr<Curator> ptr;
        const std::type_index type;
    private:
        INSCRIPTION_ACCESS;
    };

    class UnownedStoredCurator final : public StoredCurator
    {
    public:
        explicit UnownedStoredCurator(Curator* ptr, Type type);
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
