#pragma once

#include <typeindex>

#include "Exception.h"

#include "RelicID.h"
#include "Type.h"

namespace Arca
{
    class NotRegistered final : public Exception
    {
    public:
        explicit NotRegistered(
            const std::string& objectType,
            const Type& type);
        explicit NotRegistered(
            const std::string& objectType,
            const Type& type,
            const std::type_index& classType);
    private:
        [[nodiscard]] std::string BaseMessage(const std::string& objectType, const Type& type) const;
    };

    class AlreadyRegistered final : public Exception
    {
    public:
        AlreadyRegistered(
            const std::string& objectType,
            const Type& type);
        AlreadyRegistered(
            const std::string& objectType,
            const Type& type,
            const std::type_index& classType);
    private:
        [[nodiscard]] std::string BaseMessage(const std::string& objectType, const Type& type) const;
    };

    class CannotCreate final : public Exception
    {
    public:
        CannotCreate(
            const std::string& objectType,
            const Type& type);
        CannotCreate(
            const std::string& objectType,
            const Type& type,
            const std::type_index& classType);
    private:
        [[nodiscard]] std::string BaseMessage(const std::string& objectType, const Type& type) const;
    };

    class CannotDestroy final : public Exception
    {
    public:
        CannotDestroy(
            const std::string& objectType,
            const Type& type);
        CannotDestroy(
            const std::string& objectType,
            const Type& type,
            const std::type_index& classType);
    private:
        [[nodiscard]] std::string BaseMessage(const std::string& objectType, const Type& type) const;
    };

    class CannotFind final : public Exception
    {
    public:
        CannotFind(
            const std::string& objectType,
            const Type& type);
        CannotFind(
            const std::string& objectType,
            const Type& type,
            const std::type_index& classType);
    private:
        [[nodiscard]] std::string BaseMessage(const std::string& objectType, const Type& type) const;
    };

    class CannotModifyShards final : public Exception
    {
    public:
        CannotModifyShards(
            const std::string& objectType,
            RelicID id);
    };

    class CannotParentRelic final : public Exception
    {
    public:
        explicit CannotParentRelic(const std::string& message);
    };

    class IncorrectRegisteredCuratorType final : public Exception
    {
    public:
        IncorrectRegisteredCuratorType();
    };
}
