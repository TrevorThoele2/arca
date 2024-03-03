#pragma once

#include "CommandResult.h"
#include "CommandHandlerException.h"

#include "Pipeline.h"
#include "CuratorStage.h"
#include "HasCommandHandleMethodWithStage.h"
#include "ReliquaryCurators.h"

namespace Arca
{
    class ReliquaryCurators;

    class CommandHandlerBase
    {
    public:
        virtual ~CommandHandlerBase() = 0;

        virtual void SetupPipeline(const Pipeline& pipeline) = 0;
    };

    template<class CommandT, bool hasResult>
    class CommandHandler;

    template<class CommandT>
    class CommandHandler<CommandT, true> : public CommandHandlerBase
    {
    public:
        template<class CuratorT>
        void LinkTo();

        void SetupPipeline(const Pipeline& pipeline) override;

        command_result_t<CommandT> Handle(const CommandT& command, ReliquaryCurators& curators);
    private:
        class LinkBase
        {
        public:
            virtual ~LinkBase() = 0;

            virtual command_result_t<CommandT> Handle(const CommandT& command, ReliquaryCurators& curators) = 0;
        };

        template<class CuratorT>
        class Link : public LinkBase
        {
        public:
            command_result_t<CommandT> Handle(const CommandT& command, ReliquaryCurators& curators) override;
        };

        using LinkPtr = std::unique_ptr<LinkBase>;
        LinkPtr link;
    };

    template<class CommandT>
    class CommandHandler<CommandT, false> : public CommandHandlerBase
    {
    public:
        CommandHandler();

        template<class CuratorT>
        void LinkTo();

        void SetupPipeline(const Pipeline& pipeline) override;

        void Handle(const CommandT& command, ReliquaryCurators& curators);
    private:
        class LinkBase
        {
        public:
            virtual ~LinkBase() = 0;

            virtual void Handle(const CommandT& command, CuratorStage& stage, ReliquaryCurators& curators) = 0;

            [[nodiscard]] virtual TypeName CuratorTypeName() const = 0;
        };

        template<class CuratorT>
        class Link final : public LinkBase
        {
        public:
            void Handle(const CommandT& command, CuratorStage& stage, ReliquaryCurators& curators) override;

            [[nodiscard]] TypeName CuratorTypeName() const override;
        private:
            template<class U, std::enable_if_t<has_command_handle_method_with_stage_v<CuratorT, U>, int> = 0>
            void HandleImpl(CuratorT& curator, const CommandT& command, CuratorStage& stage);
            template<class U, std::enable_if_t<!has_command_handle_method_with_stage_v<CuratorT, U>, int> = 0>
            void HandleImpl(CuratorT& curator, const CommandT& command, CuratorStage& stage);
        };

        using LinkPtr = std::unique_ptr<LinkBase>;
        using LinkStage = std::vector<LinkPtr>;
        using LinkPipeline = std::vector<LinkStage>;
        LinkPipeline pipeline;
    };

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
    void CommandHandler<CommandT, false>::SetupPipeline(const Pipeline & pipeline)
    {
        std::unordered_map<TypeName, LinkPtr> links;
        for (auto& link : this->pipeline[0])
            links.emplace(link->CuratorTypeName(), std::move(link));

        this->pipeline.clear();

        for (auto& inputStage : pipeline)
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