#include "ReliquaryException.h"

#include <Chroma/StringUtility.h>

namespace Arca
{
    AttemptedLoadWhileInitialized::AttemptedLoadWhileInitialized() :
        Exception("Loading an already initialized reliquary is not possible.")
    {}

    IncorrectRegisteredCuratorType::IncorrectRegisteredCuratorType() :
        Exception("The type given does not match the runtime type.")
    {}

    NotRegistered::NotRegistered(const std::string& type, const std::string& name) :
        Exception("The " + type + " (" + name + ") was not registered.")
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

    CannotFindVessel::CannotFindVessel(VesselID id) :
        Exception("The vessel with id (" + ::Chroma::ToString(id) + ") cannot be found.")
    {}

    VesselAlreadyParented::VesselAlreadyParented(VesselID id) :
        Exception("The vessel with id (" + ::Chroma::ToString(id) + ") has already been parented.")
    {}

    CannotParentVesselToSelf::CannotParentVesselToSelf(VesselID id) :
        Exception("The vessel with id (" + ::Chroma::ToString(id) + ") was attempted to be parented to itself.")
    {}

    InvalidCuratorPipeline::InvalidCuratorPipeline(const std::string& reason) :
        Exception(reason)
    {}
}