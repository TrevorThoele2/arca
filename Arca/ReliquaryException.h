#pragma once

#include "Exception.h"

namespace Arca
{
    class NotInitialized final : public Exception
    {
    public:
        NotInitialized();
    };

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
        NotRegistered();
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
}