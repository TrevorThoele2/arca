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
    };

    class CannotModify final : public Exception
    {
    public:
        CannotModify(
            const std::string& objectType,
            const RelicID id);
    };

    class CannotCreateShard final : public Exception
    {
    public:
        CannotCreateShard();
    };

    class CannotDestroyShard final : public Exception
    {
    public:
        CannotDestroyShard();
    };

    class CannotFindRelic final : public Exception
    {
    public:
        explicit CannotFindRelic(RelicID id);
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
