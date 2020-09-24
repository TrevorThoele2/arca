#pragma once

#include "CommandResult.h"
#include "TypeFor.h"

namespace Arca
{
    class ReliquaryCurators;

    class CommandHandlerBase
    {
    public:
        virtual ~CommandHandlerBase() = 0;
    };

    template<class CommandT, bool hasResult>
    class CommandHandler;

    template<class CommandT>
    class CommandHandler<CommandT, true> : public CommandHandlerBase
    {
    public:
        template<class CuratorT>
        void LinkTo();

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
        template<class CuratorT>
        void LinkTo();

        void Handle(const CommandT& command, ReliquaryCurators& curators);
    private:
        class LinkBase
        {
        public:
            virtual ~LinkBase() = 0;

            virtual void Handle(const CommandT& command, ReliquaryCurators& curators) = 0;
        };

        template<class CuratorT>
        class Link : public LinkBase
        {
        public:
            void Handle(const CommandT& command, ReliquaryCurators& curators) override;
        };

        using LinkPtr = std::unique_ptr<LinkBase>;
        using Links = std::vector<LinkPtr>;
        Links links;
    };
}