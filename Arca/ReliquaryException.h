#pragma once

#include "Exception.h"

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

    class CannotCreateRelic final : public Exception
    {
    public:
        CannotCreateRelic();
    };

    class CannotDestroyRelic final : public Exception
    {
    public:
        CannotDestroyRelic();
    };

    class CannotFindVessel final : public Exception
    {
    public:
        CannotFindVessel();
    };

    class VesselAlreadyParented final : public Exception
    {
    public:
        VesselAlreadyParented();
    };
}