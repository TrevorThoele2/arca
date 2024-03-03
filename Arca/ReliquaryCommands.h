#pragma once

#include "ReliquaryComponent.h"
#include "CommandReturn.h"
#include "IsCommand.h"
#include "IsCurator.h"

#include "Create.h"
#include "CreateWith.h"
#include "CreateChild.h"
#include "CreateChildWith.h"
#include "Destroy.h"
#include "Clear.h"

namespace Arca
{
    class ReliquaryCommands : public ReliquaryComponent
    {
    public:
        template<class CommandT, std::enable_if_t<is_command_v<CommandT> && std::is_void_v<command_return_t<CommandT>>, int> = 0>
        void Do(const CommandT& command);
        template<class CommandT, std::enable_if_t<is_command_v<CommandT> && !std::is_void_v<command_return_t<CommandT>>, int> = 0>
        command_return_t<CommandT> Do(const CommandT& command);
        template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
        command_return_t<Create<T>> Do(const Create<T>& command);
        template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
        command_return_t<Create<T>> Do(const Create<T>& command);
        template<class T>
        command_return_t<CreateWith<T>> Do(const CreateWith<T>& command);
        template<class T>
        command_return_t<CreateChild<T>> Do(const CreateChild<T>& command);
        template<class T>
        command_return_t<CreateChildWith<T>> Do(const CreateChildWith<T>& command);
        template<class T, std::enable_if_t<is_relic_v<T>, int> = 0>
        void Do(const Destroy<T>& command);
        template<class T, std::enable_if_t<is_shard_v<T>, int> = 0>
        void Do(const Destroy<T>& command);
        template<class T, std::enable_if_t<is_matrix_v<T>, int> = 0>
        void Do(const Destroy<T>& command);
        void Do(const Clear& command);

        template<class CuratorT, class CommandT, std::enable_if_t<is_curator_v<CuratorT> && is_command_v<CommandT>, int> = 0>
        void Link();
    public:
        ReliquaryCommands(const ReliquaryCommands& arg) = delete;
        ReliquaryCommands& operator=(const ReliquaryCommands& arg) = delete;
    private:
        class HandlerBase
        {
        public:
            virtual ~HandlerBase() = 0;
        };

        template<class CommandT>
        class Handler : public HandlerBase
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
            using LinkList = std::vector<LinkPtr>;
            LinkList links;
        };

        using HandlerPtr = std::unique_ptr<HandlerBase>;
        using HandlerMap = std::unordered_map<TypeName, HandlerPtr>;
        HandlerMap handlerMap;

        template<class T>
        Handler<T>& RequiredHandler();
    private:
        explicit ReliquaryCommands(Reliquary& owner);
        ReliquaryCommands(ReliquaryCommands&& arg) noexcept = default;
        friend Reliquary;
    };
}
