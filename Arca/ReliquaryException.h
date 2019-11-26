#pragma once

#include <typeindex>
#include "Exception.h"
#include "RelicID.h"

namespace Arca
{
    class AttemptedLoadWhileInitialized final : public Exception
    {
    public:
        AttemptedLoadWhileInitialized();
    };

    class IncorrectRegisteredCuratorType final : public Exception
    {
    public:
        IncorrectRegisteredCuratorType();
    };

    class NotRegistered final : public Exception
    {
    public:
        explicit NotRegistered(
            const std::string& type,
            const std::string& name);
        explicit NotRegistered(
            const std::string& type,
            const std::string& name,
            const std::type_index& classType);
    };

    class AlreadyRegistered final : public Exception
    {
    public:
        AlreadyRegistered(
            const std::string& type,
            const std::string& name);
        AlreadyRegistered(
            const std::string& type,
            const std::string& name,
            const std::type_index& classType);
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
}
