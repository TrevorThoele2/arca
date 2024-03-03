#include "ReliquaryException.h"

namespace Arca
{
    NotInitialized::NotInitialized() :
        Exception("The Reliquary has not been initialized.")
    {}

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
}