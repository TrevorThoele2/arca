#pragma once

#include <vector>

#include "Reliquary.h"

#include "Pipeline.h"
#include "HasHandledCommands.h"

#include "Operation.h"
#include "ReliquaryException.h"

namespace Arca
{
    class ReliquaryOrigin
    {
    public:
        ReliquaryOrigin() = default;
        ReliquaryOrigin(const ReliquaryOrigin& arg) = delete;
        ReliquaryOrigin(ReliquaryOrigin&& arg) = default;
        ReliquaryOrigin& operator=(const ReliquaryOrigin& arg) = delete;
        ReliquaryOrigin& operator=(ReliquaryOrigin&& arg) = default;
    public:
        [[nodiscard]] std::unique_ptr<Reliquary> Actualize();
    public:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        ReliquaryOrigin&& Register();
        template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        ReliquaryOrigin&& Register(ConstructorArgs&& ... constructorArgs);
        ReliquaryOrigin&& RelicStructure(const std::string& name, const RelicStructure& structure);
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ReliquaryOrigin&& Register();
    public:
        template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        ReliquaryOrigin&& Register(Args&& ... args);
        ReliquaryOrigin&& CuratorConstructionPipeline(const Pipeline& constructionPipeline);
        template<class CommandT, std::enable_if_t<is_command_v<CommandT>, int> = 0>
        ReliquaryOrigin&& CuratorCommandPipeline(const Pipeline& commandPipeline);
        ReliquaryOrigin&& CuratorCommandPipeline(const TypeName& commandType, const Pipeline& commandPipeline);
    private:
        struct TypeConstructor
        {
            TypeName typeName;

            using Factory = fu2::unique_function<void(Reliquary&)>;
            Factory factory;

            TypeConstructor(TypeName typeName, Factory&& factory);
            TypeConstructor(TypeConstructor&& arg) noexcept;
            TypeConstructor& operator=(TypeConstructor&& arg) noexcept;
        };
        using TypeConstructorList = std::vector<TypeConstructor>;
    private:
        TypeConstructorList relicList;

        template<class RelicT>
        [[nodiscard]] bool IsRelicRegistered() const;
    private:
        TypeConstructorList globalRelicList;

        template<class RelicT, class... ConstructorArgs>
        void GlobalRelicCommon(ConstructorArgs&& ... constructorArgs);

        template<class RelicT>
        [[nodiscard]] bool IsGlobalRelicRegistered() const;
    private:
        using NamedRelicStructure = ReliquaryRelicStructures::Named;
        using NamedRelicStructureList = std::vector<NamedRelicStructure>;
        NamedRelicStructureList namedRelicStructureList;
    private:
        TypeConstructorList shardList;

        template<class ShardT>
        [[nodiscard]] bool IsShardRegistered() const;
    private:
        TypeConstructorList curatorList;

        Pipeline curatorConstructionPipeline;

        using PipelineMap = std::unordered_map<TypeName, Pipeline>;
        PipelineMap curatorCommandPipelines;

        void PushAllCuratorsTo(Reliquary& reliquary);

        template<class Curator>
        [[nodiscard]] bool IsCuratorRegistered() const;

        void ValidateCuratorPipeline(const Pipeline& pipeline) const;

        template<Chroma::VariadicTemplateSize i>
        struct LinkHandledCommandIterator
        {
            template<class Curator>
            static void Do(Curator& curator, ReliquaryCommands& commands);
        };

        template<class Curator, std::enable_if_t<has_handled_commands_v<Curator>, int> = 0>
        static void LinkHandledCommands(Curator& curator, Reliquary& reliquary);
        template<class Curator, std::enable_if_t<!has_handled_commands_v<Curator>, int> = 0>
        static void LinkHandledCommands(Curator& curator, Reliquary& reliquary);
    };

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    ReliquaryOrigin&& ReliquaryOrigin::Register()
    {
        const auto type = TypeFor<RelicT>();

        if (IsRelicRegistered<RelicT>())
            throw AlreadyRegistered("relic", type, typeid(RelicT));

        relicList.emplace_back(
            type.name,
            [](Reliquary& reliquary)
            {
                reliquary.relics.CreateLocalHandler<RelicT>();
            });

        return std::move(*this);
    }

    template<class RelicT, class... ConstructorArgs, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    ReliquaryOrigin&& ReliquaryOrigin::Register(ConstructorArgs&& ... constructorArgs)
    {
        if (IsGlobalRelicRegistered<RelicT>())
            throw AlreadyRegistered("global relic", TypeFor<RelicT>(), typeid(RelicT));

        GlobalRelicCommon<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...);
        return std::move(*this);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ReliquaryOrigin&& ReliquaryOrigin::Register()
    {
        const auto type = TypeFor<ShardT>();

        if (IsShardRegistered<ShardT>())
            throw AlreadyRegistered("shard", type, typeid(ShardT));

        shardList.emplace_back(
            type.name,
            [](Reliquary& reliquary)
            {
                reliquary.shards.CreateHandler<ShardT>();
            });

        return std::move(*this);
    }

    template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int>>
    ReliquaryOrigin&& ReliquaryOrigin::Register(Args&& ... args)
    {
        const auto type = TypeFor<CuratorT>();

        if (IsCuratorRegistered<CuratorT>())
            throw AlreadyRegistered("curator", type, typeid(CuratorT));

        curatorList.push_back(TypeConstructor{
            type.name,
            CreateOperation<Reliquary&>(
                [](Reliquary& reliquary, auto&& ... args)
                {
                    reliquary.curators.CreateHandler<CuratorT>(std::forward<decltype(args)>(args)...);

                    auto& curator = reliquary.curators.Find<CuratorT>();
                    LinkHandledCommands<CuratorT>(curator, reliquary);
                },
                std::forward<Args>(args)...) });

        return std::move(*this);
    }

    template<class CommandT, std::enable_if_t<is_command_v<CommandT>, int>>
    ReliquaryOrigin&& ReliquaryOrigin::CuratorCommandPipeline(const Pipeline& commandPipeline)
    {
        return CuratorCommandPipeline(TypeFor<CommandT>().name, commandPipeline);
    }

    template<class RelicT>
    [[nodiscard]] bool ReliquaryOrigin::IsRelicRegistered() const
    {
        const auto type = TypeFor<RelicT>();
        const auto found = std::find_if(
            relicList.begin(),
            relicList.end(),
            [type](const TypeConstructor& constructor)
            {
                return constructor.typeName == type.name;
            });
        return found != relicList.end();
    }

    template<class RelicT, class... ConstructorArgs>
    void ReliquaryOrigin::GlobalRelicCommon(ConstructorArgs&& ... constructorArgs)
    {
        globalRelicList.push_back(TypeConstructor{
            TypeFor<RelicT>().name,
            CreateOperation<Reliquary&>(
                [](Reliquary& reliquary, auto&& ... constructorArgs)
                {
                    reliquary.relics.CreateGlobalHandler<RelicT>(
                        std::forward<decltype(constructorArgs)>(constructorArgs)...);
                },
                std::forward<ConstructorArgs>(constructorArgs)...) });
    }

    template<class RelicT>
    bool ReliquaryOrigin::IsGlobalRelicRegistered() const
    {
        const auto type = TypeFor<RelicT>();
        const auto found = std::find_if(
            globalRelicList.begin(),
            globalRelicList.end(),
            [type](const TypeConstructor& constructor)
            {
                return constructor.typeName == type.name;
            });
        return found != globalRelicList.end();
    }

    template<class ShardT>
    bool ReliquaryOrigin::IsShardRegistered() const
    {
        const auto type = TypeFor<ShardT>();
        const auto found = std::find_if(
            shardList.begin(),
            shardList.end(),
            [type](const TypeConstructor& constructor)
            {
                return constructor.typeName == type.name;
            });
        return found != shardList.end();
    }

    template<class CuratorT>
    bool ReliquaryOrigin::IsCuratorRegistered() const
    {
        auto type = TypeFor<CuratorT>();
        return std::find_if(
            curatorList.begin(),
            curatorList.end(),
            [](const TypeConstructor& entry)
            {
                return entry.typeName == TypeFor<CuratorT>().name;
            }) != curatorList.end();
    }

    template<Chroma::VariadicTemplateSize i>
    template<class Curator>
    void ReliquaryOrigin::LinkHandledCommandIterator<i>::Do(Curator& curator, ReliquaryCommands& commands)
    {
        using CommandT = typename Traits<Curator>::HandledCommands::template Parameter<i>::Type;
        commands.Link<Curator, CommandT>();
    }

    template<class Curator, std::enable_if_t<has_handled_commands_v<Curator>, int>>
    void ReliquaryOrigin::LinkHandledCommands(Curator& curator, Reliquary& reliquary)
    {
        static_assert(
            Traits<Curator>::HandledCommands::AllUnique::value,
            "A Curator should only declare each handled command once.");

        Chroma::IterateRange<
            Chroma::VariadicTemplateSize,
            LinkHandledCommandIterator,
            Traits<Curator>::HandledCommands::count - 1>
            (curator, reliquary.commands);
    }

    template<class Curator, std::enable_if_t<!has_handled_commands_v<Curator>, int>>
    void ReliquaryOrigin::LinkHandledCommands(Curator& curator, Reliquary& reliquary)
    {}
}