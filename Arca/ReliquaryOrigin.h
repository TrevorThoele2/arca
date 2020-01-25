#pragma once

#include <vector>

#include "Reliquary.h"

#include "Pipeline.h"

#include "Curator.h"

#include "Initialize.h"

namespace Arca
{
    class ReliquaryOrigin
    {
    public:
        ReliquaryOrigin();
        ReliquaryOrigin(const ReliquaryOrigin& arg);
        ReliquaryOrigin(ReliquaryOrigin&& arg) = default;
        ReliquaryOrigin& operator=(const ReliquaryOrigin& arg);
        ReliquaryOrigin& operator=(ReliquaryOrigin&& arg) = default;
    public:
        [[nodiscard]] std::unique_ptr<Reliquary> Actualize() const;
    public:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int> = 0>
        ReliquaryOrigin& Register();
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        ReliquaryOrigin& Register(InitializeArgs&& ... initializeArgs);
        template<class InterfaceT>
        ReliquaryOrigin& Compute(std::function<InterfaceT(Reliquary&)> computation);
        ReliquaryOrigin& RelicStructure(const std::string& name, const RelicStructure& structure);
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ReliquaryOrigin& Register();
    public:
        template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        ReliquaryOrigin& Register(Args&& ... args);
        ReliquaryOrigin& CuratorPipeline(const Pipeline& pipeline);
        ReliquaryOrigin& CuratorPipeline(const Pipeline& initialization, const Pipeline& work);
    private:
        struct TypeConstructor
        {
            TypeName typeName;
            std::function<void(Reliquary&)> factory;

            TypeConstructor(TypeName typeName, std::function<void(Reliquary&)>&& factory);
        };
        using TypeConstructorList = std::vector<TypeConstructor>;
    private:
        TypeConstructorList relicList;

        template<class RelicT>
        [[nodiscard]] bool IsRelicRegistered() const;
    private:
        TypeConstructorList globalRelicList;

        using GlobalComputationInitializer = std::function<void(Reliquary&)>;
        using GlobalComputationInitializerMap = std::unordered_map<std::type_index, GlobalComputationInitializer>;
        GlobalComputationInitializerMap globalComputationInitializerMap;

        template<class RelicT, class... InitializeArgs>
        void GlobalRelicCommon(InitializeArgs&& ... initializeArgs);

        template<class RelicT>
        [[nodiscard]] bool IsGlobalRelicRegistered() const;
        template<class T>
        [[nodiscard]] bool IsGlobalComputationRegistered() const;
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

        Pipeline curatorInitializationPipeline;
        Pipeline curatorWorkPipeline;

        std::vector<Curator*> PushAllCuratorsTo(Reliquary& reliquary) const;

        template<class Curator>
        [[nodiscard]] bool IsCuratorRegistered() const;

        static ReliquaryCurators::Pipeline TransformCuratorPipeline(
            Reliquary& reliquary,
            const Pipeline& toTransform,
            const std::vector<Arca::Curator*>& allCurators);
    };

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Register()
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

        return *this;
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Register(InitializeArgs&& ... initializeArgs)
    {
        if (IsGlobalRelicRegistered<RelicT>())
            throw AlreadyRegistered("global relic", TypeFor<RelicT>(), typeid(RelicT));

        GlobalRelicCommon<RelicT>(std::forward<InitializeArgs>(initializeArgs)...);
        return *this;
    }

    template<class InterfaceT>
    ReliquaryOrigin& ReliquaryOrigin::Compute(std::function<InterfaceT(Reliquary&)> computation)
    {
        const std::type_index interfaceType = typeid(InterfaceT);

        if (IsGlobalComputationRegistered<InterfaceT>())
            throw AlreadyRegistered("global computation", interfaceType);

        globalComputationInitializerMap.emplace(
            interfaceType,
            [interfaceType, computation](Reliquary& reliquary)
            {
                reliquary.relics.globalComputationMap.emplace(
                    interfaceType,
                    [computation](Reliquary& reliquary) -> std::any
                    {
                        auto transformed = computation(reliquary);
                        return transformed;
                    });
            });

        return *this;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Register()
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

        return *this;
    }

    template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Register(Args&& ... args)
    {
        const auto type = TypeFor<CuratorT>();

        if (IsCuratorRegistered<CuratorT>())
            throw AlreadyRegistered("curator", type, typeid(CuratorT));

        const auto factory = [args = std::make_tuple(std::forward<Args>(args) ...)](Reliquary& reliquary) mutable
        {
            return std::apply(
                [&reliquary](auto&& ... args)
                {
                    reliquary.curators.CreateHandler<CuratorT>(std::forward<Args>(args)...);
                },
                args);
        };
        curatorList.emplace_back(type.name, factory);

        return *this;
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

    template<class RelicT, class... InitializeArgs>
    void ReliquaryOrigin::GlobalRelicCommon(InitializeArgs&& ... initializeArgs)
    {
        const auto factory = [args = std::make_tuple(std::forward<InitializeArgs>(initializeArgs) ...)](Reliquary& reliquary) mutable
        {
            return std::apply(
                [&reliquary](auto ... initializeArgs)
                {
                    reliquary.relics.CreateGlobalHandler<RelicT>(std::forward<InitializeArgs>(initializeArgs)...);
                },
                args);
        };
        globalRelicList.emplace_back(TypeFor<RelicT>().name, factory);
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

    template<class T>
    bool ReliquaryOrigin::IsGlobalComputationRegistered() const
    {
        const std::type_index type(typeid(T));
        const auto found = globalComputationInitializerMap.find(type);
        return found != globalComputationInitializerMap.end();
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
}