#pragma once

#include "ReliquaryCommands.h"

namespace Arca
{
    template<class CommandT, std::enable_if_t<is_command_v<CommandT> && std::is_void_v<command_return_t<CommandT>>, int>>
    void ReliquaryCommands::Do(const CommandT& command)
    {
        auto& handler = RequiredHandler<CommandT>();
        handler.Handle(command, Curators());
    }

    template<class CommandT, std::enable_if_t<is_command_v<CommandT> && !std::is_void_v<command_return_t<CommandT>>, int>>
    command_return_t<CommandT> ReliquaryCommands::Do(const CommandT& command)
    {
        auto& handler = RequiredHandler<CommandT>();
        return handler.Handle(command, Curators());
    }

    template<class T, std::enable_if_t<is_relic_v<T>, int>>
    command_return_t<Create<T>> ReliquaryCommands::Do(const Create<T>& command)
    {
        return command.function(Relics());
    }

    template<class T, std::enable_if_t<is_shard_v<T>, int>>
    command_return_t<Create<T>> ReliquaryCommands::Do(const Create<T>& command)
    {
        return command.function(Shards());
    }

    template<class T>
    command_return_t<CreateWith<T>> ReliquaryCommands::Do(const CreateWith<T>& command)
    {
        return command.function(Relics());
    }

    template<class T>
    command_return_t<CreateChild<T>> ReliquaryCommands::Do(const CreateChild<T>& command)
    {
        return command.function(Relics());
    }

    template<class T>
    command_return_t<CreateChildWith<T>> ReliquaryCommands::Do(const CreateChildWith<T>& command)
    {
        return command.function(Relics());
    }

    template<class T, std::enable_if_t<is_relic_v<T>, int>>
    void ReliquaryCommands::Do(const Destroy<T>& command)
    {
        Relics().Destroy<T>(command.id);
    }

    template<class T, std::enable_if_t<is_shard_v<T>, int>>
    void ReliquaryCommands::Do(const Destroy<T>& command)
    {
        Shards().Destroy<T>(command.id);
    }

    template<class T, std::enable_if_t<is_matrix_v<T>, int>>
    void ReliquaryCommands::Do(const Destroy<T>& command)
    {
        Matrices().Destroy<T>(command.id);
    }

    template<class CuratorT, class CommandT, std::enable_if_t<is_curator_v<CuratorT> && is_command_v<CommandT>, int>>
    void ReliquaryCommands::Link()
    {
        const auto type = TypeFor<CommandT>();
        auto handler = handlerMap.find(type.name);
        if (handler == handlerMap.end())
            handler = handlerMap.emplace(type.name, std::make_unique<Handler<CommandT>>()).first;

        auto castedHandler = static_cast<Handler<CommandT>*>(handler->second.get());
        castedHandler->template LinkTo<CuratorT>();
    }

    template<class CommandT>
    template<class CuratorT>
    void ReliquaryCommands::Handler<CommandT>::LinkTo()
    {
        links.push_back(std::make_unique<Link<CuratorT>>());
    }

    template<class CommandT>
    void ReliquaryCommands::Handler<CommandT>::Handle(const CommandT& command, ReliquaryCurators& curators)
    {
        for (auto& loop : links)
            loop->Handle(command, curators);
    }

    template<class CommandT>
    template<class CuratorT>
    void ReliquaryCommands::Handler<CommandT>::Link<CuratorT>::Handle(const CommandT& command, ReliquaryCurators& curators)
    {
        curators.Find<CuratorT>().Handle(command);
    }

    template<class CommandT>
    ReliquaryCommands::Handler<CommandT>::LinkBase::~LinkBase() = default;

    template<class T>
    auto ReliquaryCommands::RequiredHandler() -> Handler<T>&
    {
        const auto type = TypeFor<T>();
        auto found = handlerMap.find(type.name);
        if (found == handlerMap.end())
            throw NotRegistered(type, typeid(T));

        return *static_cast<Handler<T>*>(found->second.get());
    }
}