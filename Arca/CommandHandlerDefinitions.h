#pragma once

#include "CommandHandler.h"

namespace Arca
{
    template<class CommandT>
    template<class CuratorT>
    void CommandHandler<CommandT, true>::LinkTo()
    {
        if (link)
            throw CommandWithReturnValueAlreadyLinked(TypeFor<CommandT>());

        link = std::move(std::make_unique<Link<CuratorT>>());
    }

    template<class CommandT>
    CommandHandler<CommandT, true>::LinkBase::~LinkBase() = default;

    template<class CommandT>
    command_result_t<CommandT> CommandHandler<CommandT, true>::Handle(const CommandT& command, ReliquaryCurators& curators)
    {
        return link->Handle(command, curators);
    }

    template<class CommandT>
    template<class CuratorT>
    command_result_t<CommandT> CommandHandler<CommandT, true>::Link<CuratorT>::Handle(const CommandT& command, ReliquaryCurators& curators)
    {
        return curators.Find<CuratorT>().Handle(command);
    }

    template<class CommandT>
    template<class CuratorT>
    void CommandHandler<CommandT, false>::LinkTo()
    {
        links.push_back(std::move(std::make_unique<Link<CuratorT>>()));
    }

    template<class CommandT>
    CommandHandler<CommandT, false>::LinkBase::~LinkBase() = default;

    template<class CommandT>
    void CommandHandler<CommandT, false>::Handle(const CommandT& command, ReliquaryCurators& curators)
    {
        for (auto& link : links)
            link->Handle(command, curators);
    }

    template<class CommandT>
    template<class CuratorT>
    void CommandHandler<CommandT, false>::Link<CuratorT>::Handle(const CommandT& command, ReliquaryCurators& curators)
    {
        curators.Find<CuratorT>().Handle(command);
    }
}