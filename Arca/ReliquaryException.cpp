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

    NotRegistered::NotRegistered(
        const std::string& type,
        const std::string& name)
        :
        Exception(
            "The " + type + " (" + name + ") was not registered.")
    {}

    NotRegistered::NotRegistered(
        const std::string& type,
        const std::string& name,
        const std::type_index& classType)
        :
        Exception(
            "The " + type + " (" + name + ") was not registered. " +
            "The class name is: \"" + classType.name() + "\".")
    {}

    AlreadyRegistered::AlreadyRegistered(
        const std::string& type,
        const std::string& name)
        :
        Exception(
            "The " + type + " (" + name + ") was already registered.")
    {}

    AlreadyRegistered::AlreadyRegistered(
        const std::string& type,
        const std::string& name,
        const std::type_index& classType)
        :
        Exception(
            "The " + type + " (" + name + ") was already registered. " +
            "The class name is: \"" + classType.name() + "\".")
    {}

    CannotCreateShard::CannotCreateShard() :
        Exception("A shard cannot be created.")
    {}

    CannotDestroyShard::CannotDestroyShard() :
        Exception("A shard cannot be destroyed.")
    {}

    CannotFindRelic::CannotFindRelic(RelicID id) :
        Exception("The relic with id (" + ::Chroma::ToString(id) + ") cannot be found.")
    {}

    CannotParentRelic::CannotParentRelic(const std::string& message) : Exception(message)
    {}
}