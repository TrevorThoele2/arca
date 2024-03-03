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
}
