#include "ReliquaryException.h"

namespace Arca
{
    AttemptedLoadWhileInitialized::AttemptedLoadWhileInitialized() :
        Exception("Loading an already initialized reliquary is not possible.")
    {}

    IncorrectRegisteredCuratorType::IncorrectRegisteredCuratorType() :
        Exception("The type given does not match the runtime type.")
    {}

    NotRegistered::NotRegistered() :
        Exception("This relic type was not registered.")
    {}

    CannotRegister::CannotRegister() :
        Exception("Cannot register types after the Reliquary has been initialized.")
    {}

    AlreadyRegistered::AlreadyRegistered() :
        Exception("A relic has already been registered with that type name.")
    {}

    CannotCreateRelic::CannotCreateRelic() :
        Exception("A relic cannot be created.")
    {}

    CannotDestroyRelic::CannotDestroyRelic() :
        Exception("A relic cannot be destroyed.")
    {}

    CannotFindVessel::CannotFindVessel() :
        Exception("A vessel cannot be found.")
    {}

    VesselAlreadyParented::VesselAlreadyParented() :
        Exception("This vessel has already been parented. Reparenting is not supported.")
    {}
}