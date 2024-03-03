#pragma once

#include "Exception.h"
#include "VesselID.h"

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
        explicit CannotFindVessel(VesselID id);
    };

    class VesselAlreadyParented final : public Exception
    {
    public:
        explicit VesselAlreadyParented(VesselID id);
    };

    class CannotParentVesselToSelf final : public Exception
    {
    public:
        explicit CannotParentVesselToSelf(VesselID id);
    };
}
