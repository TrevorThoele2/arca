#include "ReliquaryException.h"

#include <Chroma/StringUtility.h>
using namespace std::string_literals;

namespace Arca
{
    std::string ClassNameMessage(const std::type_index& classType)
    {
        return "The class name is: \""s + classType.name() + "\".";
    }

    NotRegistered::NotRegistered(
        const std::string& objectType,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, classType))
    {}

    NotRegistered::NotRegistered(
        const std::string& objectType,
        const Type& type)
        :
        Exception(BaseMessage(objectType, type))
    {}

    NotRegistered::NotRegistered(
        const std::string& objectType,
        const Type& type,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, type) + " " + ClassNameMessage(classType))
    {}

    std::string NotRegistered::BaseMessage(const std::string& objectType, const std::type_index& classType) const
    {
        return "The " + objectType + " (" + classType.name() + ") was not registered.";
    }

    std::string NotRegistered::BaseMessage(const std::string & objectType, const Type& type) const
    {
        return "The " + objectType + " (" + Chroma::ToString(type) + ") was not registered.";
    }

    AlreadyRegistered::AlreadyRegistered(
        const std::string& objectType,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, classType))
    {}

    AlreadyRegistered::AlreadyRegistered(
        const std::string& objectType,
        const Type& type)
        :
        Exception(BaseMessage(objectType, type))
    {}

    AlreadyRegistered::AlreadyRegistered(
        const std::string& objectType,
        const Type& type,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, type) + " " + ClassNameMessage(classType))
    {}

    std::string AlreadyRegistered::BaseMessage(const std::string& objectType, const std::type_index& classType) const
    {
        return "The " + objectType + " (" + classType.name() + ") was already registered.";
    }

    std::string AlreadyRegistered::BaseMessage(const std::string& objectType, const Type& type) const
    {
        return "The " + objectType + " (" + Chroma::ToString(type) + ") was already registered.";
    }

    CannotCreate::CannotCreate(
        const std::string& objectType,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, classType))
    {}

    CannotCreate::CannotCreate(
        const std::string& objectType,
        const Type& type)
        :
        Exception(BaseMessage(objectType, type))
    {}

    CannotCreate::CannotCreate(
        const std::string& objectType,
        const Type& type,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, type) + " " + ClassNameMessage(classType))
    {}

    std::string CannotCreate::BaseMessage(const std::string& objectType, const std::type_index& classType) const
    {
        return "The " + objectType + " (" + classType.name() + ") cannot be created.";
    }

    std::string CannotCreate::BaseMessage(const std::string& objectType, const Type& type) const
    {
        return "The " + objectType + " (" + Chroma::ToString(type) + ") cannot be created.";
    }

    CannotDestroy::CannotDestroy(
        const std::string& objectType,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, classType))
    {}

    CannotDestroy::CannotDestroy(
        const std::string& objectType,
        const Type& type)
        :
        Exception(BaseMessage(objectType, type))
    {}

    CannotDestroy::CannotDestroy(
        const std::string& objectType,
        const Type& type,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, type) + " " + ClassNameMessage(classType))
    {}

    std::string CannotDestroy::BaseMessage(const std::string& objectType, const std::type_index& classType) const
    {
        return "The " + objectType + " (" + classType.name() + ") cannot be destroyed.";
    }

    std::string CannotDestroy::BaseMessage(const std::string& objectType, const Type& type) const
    {
        return "The " + objectType + " (" + Chroma::ToString(type) + ") cannot be destroyed.";
    }

    CannotFind::CannotFind(
        const std::string& objectType,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, classType))
    {}

    CannotFind::CannotFind(
        const std::string& objectType,
        const Type& type)
        :
        Exception(BaseMessage(objectType, type))
    {}

    CannotFind::CannotFind(
        const std::string& objectType,
        const Type& type,
        const std::type_index& classType)
        :
        Exception(BaseMessage(objectType, type) + " " + ClassNameMessage(classType))
    {}

    std::string CannotFind::BaseMessage(const std::string& objectType, const std::type_index& classType) const
    {
        return "The " + objectType + " (" + classType.name() + ") cannot be found.";
    }

    std::string CannotFind::BaseMessage(const std::string& objectType, const Type& type) const
    {
        return "The " + objectType + " (" + Chroma::ToString(type) + ") cannot be found.";
    }

    CannotModifyShards::CannotModifyShards(
        const std::string& objectType,
        RelicID id)
        :
        Exception(
            "The " + objectType + " with ID (" + Chroma::ToString(id) + ") " +
            "needs to be able to have its shards modified but cannot. Is the object closed?")
    {}

    CannotParentRelic::CannotParentRelic(const std::string& message) : Exception(message)
    {}

    IncorrectRegisteredCuratorType::IncorrectRegisteredCuratorType() :
        Exception("The type given does not match the runtime type.")
    {}

    CommandWithReturnValueAlreadyLinked::CommandWithReturnValueAlreadyLinked(const Type& type) :
        Exception("The command (" + type.name + ") has already been linked and returns a value. Multiple handlers are not allowed when the command has a return value.")
    {}
}