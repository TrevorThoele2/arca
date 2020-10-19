#pragma once

#include "ReliquaryCommands.h"
#include "CommandHandlerDefinitions.h"

#include "Create.h"
#include "IdentifiedCreate.h"
#include "CreateWith.h"
#include "IdentifiedCreateWith.h"
#include "CreateChild.h"
#include "IdentifiedCreateChild.h"
#include "CreateChildWith.h"
#include "IdentifiedCreateChildWith.h"
#include "Assign.h"
#include "Destroy.h"
#include "Clear.h"

namespace Arca
{
    template<
        class CommandT,
        std::enable_if_t<
            is_command_v<CommandT>
            && !has_command_result_v<CommandT>
            && !is_self_contained_v<CommandT>, int>>
    void ReliquaryCommands::Do(const CommandT& command)
    {
        auto& handler = RequiredHandler<CommandT>();
        handler.Handle(command, Curators());
    }

    template<
        class CommandT,
        std::enable_if_t<
        is_command_v<CommandT>
            && !has_command_result_v<CommandT>
            && is_self_contained_v<CommandT>, int>>
    void ReliquaryCommands::Do(const CommandT& command)
    {
        command.Do(Owner());
    }

    template<
        class CommandT,
        std::enable_if_t<
            is_command_v<CommandT>
            && has_command_result_v<CommandT>
            && !is_self_contained_v<CommandT>, int>>
    command_result_t<CommandT> ReliquaryCommands::Do(const CommandT& command)
    {
        auto& handler = RequiredHandler<CommandT>();
        return handler.Handle(command, Curators());
    }

    template<
        class CommandT,
        std::enable_if_t<
            is_command_v<CommandT>
            && has_command_result_v<CommandT>
            && is_self_contained_v<CommandT>, int>>
        command_result_t<CommandT> ReliquaryCommands::Do(const CommandT& command)
    {
        return command.Do(Owner());
    }

    template<class CuratorT, class CommandT, std::enable_if_t<is_curator_v<CuratorT> && is_command_v<CommandT>, int>>
    void ReliquaryCommands::Link()
    {
        static_assert(!is_self_contained_v<CommandT>, "Self contained commands cannot have a curator handle them.");

        const auto type = TypeFor<CommandT>();
        auto handler = handlerMap.find(type.name);
        if (handler == handlerMap.end())
            handler = handlerMap.emplace(type.name, std::make_unique<HandlerT<CommandT>>()).first;

        auto castedHandler = static_cast<HandlerT<CommandT>*>(handler->second.get());
        castedHandler->template LinkTo<CuratorT>();
    }

    template<class T>
    auto ReliquaryCommands::RequiredHandler() -> HandlerT<T>&
    {
        const auto type = TypeFor<T>();
        auto found = handlerMap.find(type.name);
        if (found == handlerMap.end())
            throw NotRegistered(type, typeid(T));

        return *static_cast<HandlerT<T>*>(found->second.get());
    }
}