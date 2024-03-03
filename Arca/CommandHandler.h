#pragma once

#include "CommandResult.h"

#include "Pipeline.h"
#include "CuratorStage.h"
#include "HasCommandHandleMethodWithStage.h"

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
}