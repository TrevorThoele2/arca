#pragma once

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
        explicit NotRegistered(const std::string& type, const std::string& name);
    };

    class CannotRegister final : public Exception
    {
    public:
        CannotRegister();
    };

    class AlreadyRegistered final : public Exception
    {
    public:
        AlreadyRegistered();
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

    class RelicAlreadyParented final : public Exception
    {
    public:
        explicit RelicAlreadyParented(RelicID id);
    };

    class CannotParentRelicToSelf final : public Exception
    {
    public:
        explicit CannotParentRelicToSelf(RelicID id);
    };

    class InvalidCuratorPipeline final : public Exception
    {
    public:
        explicit InvalidCuratorPipeline(const std::string& reason);
    };
}
