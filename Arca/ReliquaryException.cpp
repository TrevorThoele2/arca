#include "ReliquaryException.h"

#include <Chroma/StringUtility.h>

namespace Arca
{
    NotRegistered::NotRegistered(
        const std::string& objectType,
        const Type& type)
        :
        Exception(
            "The " + objectType + " (" + ::Chroma::ToString(type) + ") was not registered.")
    {}

    NotRegistered::NotRegistered(
        const std::string& objectType,
        const Type& type,
        const std::type_index& classType)
        :
        Exception(
            "The " + objectType + " (" + ::Chroma::ToString(type) + ") was not registered. " +
            "The class name is: \"" + classType.name() + "\".")
    {}

    AlreadyRegistered::AlreadyRegistered(
        const std::string& objectType,
        const Type& type)
        :
        Exception(
            "The " + objectType + " (" + ::Chroma::ToString(type) + ") was already registered.")
    {}

    AlreadyRegistered::AlreadyRegistered(
        const std::string& objectType,
        const Type& type,
        const std::type_index& classType)
        :
        Exception(
            "The " + objectType + " (" + ::Chroma::ToString(type) + ") was already registered. " +
            "The class name is: \"" + classType.name() + "\".")
    {}

    CannotModify::CannotModify(
        const std::string& objectType,
        const RelicID id)
        :
        Exception("The " + objectType + " with ID (" + ::Chroma::ToString(id) + ") needs to be modified but cannot.")
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

    IncorrectRegisteredCuratorType::IncorrectRegisteredCuratorType() :
        Exception("The type given does not match the runtime type.")
    {}
}