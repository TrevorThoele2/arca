#pragma once

#include "CommandHandler.h"

#include "CommandHandlerException.h"

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
    void CommandHandler<CommandT, true>::SetupPipeline(const Pipeline& pipeline)
    {}

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
    CommandHandler<CommandT, false>::CommandHandler()
    {
        pipeline.push_back({});
    }

    template<class CommandT>
    template<class CuratorT>
    void CommandHandler<CommandT, false>::LinkTo()
    {
        pipeline[0].push_back(std::move(std::make_unique<Link<CuratorT>>()));
    }

    template<class CommandT>
    CommandHandler<CommandT, false>::LinkBase::~LinkBase() = default;

    template<class CommandT>
    void CommandHandler<CommandT, false>::SetupPipeline(const Pipeline& pipeline)
    {
        std::unordered_map<TypeName, LinkPtr> links;
        for (auto& link : this->pipeline[0])
            links.emplace(link->CuratorTypeName(), std::move(link));

        this->pipeline.clear();

        for(auto& inputStage : pipeline)
        {
            this->pipeline.push_back({});
            auto& stage = this->pipeline.back();
            auto inputStageTypeNames = inputStage.TypeNameList();
            stage.reserve(inputStageTypeNames.size());
            for (auto& inputType : inputStageTypeNames)
            {
                auto foundLink = links.find(inputType);
                if (foundLink == links.end())
                    throw CommandNotRegisteredInCurator(inputType, TypeFor<CommandT>().name);

                stage.push_back(std::move(foundLink->second));
            }
        }
    }

    template<class CommandT>
    void CommandHandler<CommandT, false>::Handle(const CommandT& command, ReliquaryCurators& curators)
    {
        for (auto& stage : pipeline)
        {
            CuratorStage stageHandle;
            for (auto& handler : stage)
            {
                handler->Handle(command, stageHandle, curators);
                if (stageHandle.IsAborted())
                    return;
            }
        }
    }

    template<class CommandT>
    template<class CuratorT>
    void CommandHandler<CommandT, false>::Link<CuratorT>::Handle(
        const CommandT& command, CuratorStage& stage, ReliquaryCurators& curators)
    {
        HandleImpl<CommandT>(curators.Find<CuratorT>(), command, stage);
    }

    template<class CommandT>
    template<class CuratorT>
    TypeName CommandHandler<CommandT, false>::Link<CuratorT>::CuratorTypeName() const
    {
        return TypeFor<CuratorT>().name;
    }

    template<class CommandT>
    template<class CuratorT>
    template<class U, std::enable_if_t<has_command_handle_method_with_stage_v<CuratorT, U>, int>>
    void CommandHandler<CommandT, false>::Link<CuratorT>::HandleImpl(
        CuratorT& curator, const CommandT& command, Curator::Stage& stage)
    {
        curator.Handle(command, stage);
    }

    template<class CommandT>
    template<class CuratorT>
    template<class U, std::enable_if_t<!has_command_handle_method_with_stage_v<CuratorT, U>, int>>
    void CommandHandler<CommandT, false>::Link<CuratorT>::HandleImpl(
        CuratorT& curator, const CommandT& command, Curator::Stage&)
    {
        curator.Handle(command);
    }
}