#pragma once

#include "CommandHandler.h"
#include "CommandResult.h"
#include "IsCommand.h"
#include "IsCurator.h"
#include "IsSelfContained.h"

namespace Arca
{
    class ReliquaryCurators;

    class ReliquaryCommands
    {
    public:
        template<
            class CommandT,
            std::enable_if_t<
                is_command_v<CommandT>
                && !has_command_result_v<CommandT>
                && !is_self_contained_v<CommandT>, int> = 0>
        void Do(const CommandT& command);
        template<
            class CommandT,
            std::enable_if_t<
                is_command_v<CommandT>
                && !has_command_result_v<CommandT>
                && is_self_contained_v<CommandT>, int> = 0>
            void Do(const CommandT& command);
        template<
            class CommandT,
            std::enable_if_t<
                is_command_v<CommandT>
                && has_command_result_v<CommandT>
                && !is_self_contained_v<CommandT>, int> = 0>
        command_result_t<CommandT> Do(const CommandT& command);
        template<
            class CommandT,
            std::enable_if_t<
                is_command_v<CommandT>
                && has_command_result_v<CommandT>
                && is_self_contained_v<CommandT>, int> = 0>
            command_result_t<CommandT> Do(const CommandT& command);

        template<class CuratorT, class CommandT, std::enable_if_t<is_curator_v<CuratorT> && is_command_v<CommandT>, int> = 0>
        void Link();
    public:
        ReliquaryCommands(const ReliquaryCommands& arg) = delete;
        ReliquaryCommands& operator=(const ReliquaryCommands& arg) = delete;
    public:
        template<class T>
        using HandlerT = CommandHandler<T, !std::is_void_v<command_result_t<T>>>;

        template<class T>
        HandlerT<T>& RequiredHandler();
        CommandHandlerBase& RequiredHandler(const TypeName& typeName);
    private:
        using HandlerBase = CommandHandlerBase;
        using HandlerPtr = std::unique_ptr<HandlerBase>;
        using HandlerMap = std::unordered_map<TypeName, HandlerPtr>;
        HandlerMap handlerMap;
    private:
        Reliquary* owner;
        ReliquaryCurators* curators;

        const std::string objectTypeName = "command";
    private:
        explicit ReliquaryCommands(Reliquary& owner, ReliquaryCurators& curators);
        ReliquaryCommands(ReliquaryCommands&& arg) noexcept = default;
        friend Reliquary;
    };

    template<
        class CommandT,
        std::enable_if_t<
            is_command_v<CommandT>
            && !has_command_result_v<CommandT>
            && !is_self_contained_v<CommandT>, int>>
    void ReliquaryCommands::Do(const CommandT& command)
    {
        auto& handler = RequiredHandler<CommandT>();
        handler.Handle(command, *curators);
    }

    template<
        class CommandT,
        std::enable_if_t<
            is_command_v<CommandT>
            && !has_command_result_v<CommandT>
            && is_self_contained_v<CommandT>, int>>
    void ReliquaryCommands::Do(const CommandT& command)
    {
        command.Do(*owner);
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
        return handler.Handle(command, *curators);
    }

    template<
        class CommandT,
        std::enable_if_t<
            is_command_v<CommandT>
            && has_command_result_v<CommandT>
            && is_self_contained_v<CommandT>, int>>
    command_result_t<CommandT> ReliquaryCommands::Do(const CommandT& command)
    {
        return command.Do(*owner);
    }

    template<class CuratorT, class CommandT, std::enable_if_t<is_curator_v<CuratorT>&& is_command_v<CommandT>, int>>
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
            throw NotRegistered(objectTypeName, type, typeid(T));

        return *static_cast<HandlerT<T>*>(found->second.get());
    }
}
