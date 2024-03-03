#pragma once

#include <vector>

#include "Reliquary.h"

#include "Pipeline.h"

#include "Curator.h"
#include "CuratorProvider.h"

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
        ReliquaryOrigin& Type();
        template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int> = 0>
        ReliquaryOrigin& Type(InitializeArgs&& ... initializeArgs);
        template<
            class InterfaceT,
            class BackingT,
            class... BackingInitializeT,
            std::enable_if_t<is_relic_v<BackingT> && is_global_v<BackingT>, int> = 0>
        ReliquaryOrigin& Alias(
            std::function<InterfaceT(BackingT&)> transformation,
            BackingInitializeT&& ... initializeArgs);
        ReliquaryOrigin& RelicStructure(const std::string& name, const RelicStructure& structure);
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ReliquaryOrigin& Type();
    public:
        template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        ReliquaryOrigin& Type(Args&& ... args);
        template<class AsT, class ProvidedT, std::enable_if_t<is_curator_v<ProvidedT> && std::is_base_of_v<ProvidedT, Arca::Curator>, int> = 0>
        ReliquaryOrigin& Type(ProvidedT* use);
        ReliquaryOrigin& CuratorPipeline(const Pipeline& pipeline);
        ReliquaryOrigin& CuratorPipeline(const Pipeline& initialization, const Pipeline& work);
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        ReliquaryOrigin& Type();
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

        using GlobalRelicAliasInitializer = std::function<void(Reliquary&)>;
        using GlobalRelicAliasInitializerMap = std::unordered_map<std::type_index, GlobalRelicAliasInitializer>;
        GlobalRelicAliasInitializerMap globalRelicAliasInitializerMap;

        template<class RelicT, class... InitializeArgs>
        void GlobalRelicCommon(InitializeArgs&& ... initializeArgs);

        template<class RelicT>
        [[nodiscard]] bool IsGlobalRelicRegistered() const;
        template<class T>
        [[nodiscard]] bool IsGlobalRelicAliasRegistered() const;
    private:
        using NamedRelicStructure = ReliquaryRelicStructures::Named;
        using NamedRelicStructureList = std::vector<NamedRelicStructure>;
        NamedRelicStructureList namedRelicStructureList;
    private:
        TypeConstructorList shardList;

        template<class ShardT>
        [[nodiscard]] bool IsShardRegistered() const;
    private:
        using CuratorProviderPtr = std::unique_ptr<CuratorProviderBase>;
        using CuratorProviderMap = std::unordered_map<TypeName, CuratorProviderPtr>;
        CuratorProviderMap curatorProviders;

        Pipeline curatorInitializationPipeline;
        Pipeline curatorWorkPipeline;

        using CuratorSerializationTypesFactory = void(*)(Reliquary&);
        using CuratorSerializationTypesFactoryList = std::vector<CuratorSerializationTypesFactory>;
        CuratorSerializationTypesFactoryList curatorSerializationTypesFactoryList;

        std::vector<Arca::Curator*> PushAllCuratorsTo(Reliquary& reliquary) const;

        template<class Curator, class CuratorProvider, class... Args>
        void CuratorCommon(Args&& ... args);
        template<class Curator>
        [[nodiscard]] bool IsCuratorRegistered() const;

        static ReliquaryCurators::Pipeline TransformCuratorPipeline(
            Reliquary& reliquary,
            const Pipeline& toTransform,
            const std::vector<Arca::Curator*>& allCurators);
    private:
        struct SignalConstructor
        {
            std::type_index type;
            void(*factory)(Reliquary&);

            SignalConstructor(std::type_index type, void(*factory)(Reliquary&));
        };
        std::vector<SignalConstructor> signalList;

        template<class SignalT>
        [[nodiscard]] bool IsSignalRegistered() const;
    };

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT> && is_local_v<RelicT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Type()
    {
        const auto type = TypeFor<RelicT>();

        if (IsRelicRegistered<RelicT>())
            throw AlreadyRegistered("relic", type, typeid(RelicT));

        const auto factory = [](Reliquary& reliquary)
        {
            const auto type = TypeFor<RelicT>();
            reliquary.relics.batchSources.map.emplace(
                type.name,
                std::make_unique<BatchSource<RelicT>>(reliquary));
            if (HasScribe<RelicT>())
            {
                reliquary.relics.serializers.push_back(
                    KnownPolymorphicSerializer
                    {
                        type.name,
                        [](Reliquary& reliquary, ::Inscription::BinaryArchive& archive)
                        {
                            auto batchSource = reliquary.relics.batchSources.Find<RelicT>();
                            archive(*batchSource);
                        },
                        [](::Inscription::BinaryArchive& archive)
                        {
                            return ::Inscription::InputTypesFor<RelicT>(archive);
                        }
                    });
            }
        };
        relicList.emplace_back(type.name, factory);

        return *this;
    }

    template<class RelicT, class... InitializeArgs, std::enable_if_t<is_relic_v<RelicT> && is_global_v<RelicT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Type(InitializeArgs&& ... initializeArgs)
    {
        GlobalRelicCommon<RelicT>(std::forward<InitializeArgs>(initializeArgs)...);
        return *this;
    }

    template<
        class InterfaceT,
        class BackingT,
        class... BackingInitializeT,
        std::enable_if_t<is_relic_v<BackingT> && is_global_v<BackingT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Alias(
        std::function<InterfaceT(BackingT&)> transformation,
        BackingInitializeT&& ... initializeArgs)
    {
        const std::type_index interfaceType = typeid(InterfaceT);
        const auto backingType = TypeFor<BackingT>();

        if (IsGlobalRelicAliasRegistered<InterfaceT>())
            throw AlreadyRegistered("global alias", typeid(InterfaceT));

        GlobalRelicCommon<BackingT>(std::forward<BackingInitializeT>(initializeArgs)...);

        globalRelicAliasInitializerMap.emplace(
            interfaceType,
            [interfaceType, transformation](Reliquary& reliquary)
            {
                reliquary.relics.globalRelicAliasMap.emplace(
                    interfaceType,
                    [transformation](Reliquary& reliquary) -> std::any
                    {
                        const auto backingType = TypeFor<BackingT>();
                        auto found = reliquary.relics.globalMap.find(backingType.name);
                        if (found == reliquary.relics.globalMap.end())
                            throw reliquary.relics.NotRegistered(backingType, typeid(BackingT));

                        auto backing = reinterpret_cast<BackingT*>(found->second.storage.get());
                        auto transformed = transformation(*backing);
                        return transformed;
                    });
            });

        return *this;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Type()
    {
        const auto type = TypeFor<ShardT>();

        if (IsShardRegistered<ShardT>())
            throw AlreadyRegistered("shard", type, typeid(ShardT));

        const auto factory = [type](Reliquary& reliquary)
        {
            reliquary.shards.batchSources.map.emplace(
                type.name,
                std::make_unique<BatchSource<ShardT>>());
            reliquary.shards.batchSources.constMap.emplace(
                type.name,
                std::make_unique<BatchSource<const ShardT>>());

            reliquary.shards.factoryMap.emplace(
                type.name,
                [](Reliquary& reliquary, RelicID id, bool isConst)
                {
                    const auto creator = [id, &reliquary](auto found)
                    {
                        auto added = found->Add(id);
                        reliquary.shards.AttemptAddToEitherBatches(id, *added);
                        reliquary.Raise<Created>(Handle(id, reliquary, TypeFor<ShardT>(), HandleObjectType::Shard));
                    };

                    isConst
                        ? creator(reliquary.shards.batchSources.Find<const ShardT>())
                        : creator(reliquary.shards.batchSources.Find<ShardT>());
                });

            if (HasScribe<ShardT>())
            {
                reliquary.shards.serializers.push_back(
                    KnownPolymorphicSerializer
                    {
                        type.name,
                        [](Reliquary& reliquary, ::Inscription::BinaryArchive& archive)
                        {
                            auto batchSource = reliquary.shards.batchSources.Find<ShardT>();
                            archive(*batchSource);

                            auto constBatchSource = reliquary.shards.batchSources.Find<const ShardT>();
                            archive(*constBatchSource);
                        },
                        [](::Inscription::BinaryArchive& archive)
                        {
                            return ::Inscription::InputTypesFor<ShardT>(archive);
                        }
                    });
            }
        };

        shardList.emplace_back(type.name, std::move(factory));

        return *this;
    }

    template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Type(Args&& ... args)
    {
        CuratorCommon<CuratorT, CuratorProvider<CuratorT, Args...>>(std::make_tuple(std::forward<Args>(args)...));

        return *this;
    }

    template<class AsT, class ProvidedT, std::enable_if_t<is_curator_v<ProvidedT> && std::is_base_of_v<ProvidedT, Curator>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Type(ProvidedT* use)
    {
        if (!dynamic_cast<AsT>(use))
            throw IncorrectRegisteredCuratorType();

        CuratorCommon<AsT, ExternalCuratorProvider>(use, TypeFor<ProvidedT>());

        return *this;
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Type()
    {
        const auto type = std::type_index(typeid(SignalT));

        if (IsSignalRegistered<SignalT>())
            throw AlreadyRegistered("signal", Arca::Type(type.name()));

        const auto factory = [](Reliquary& reliquary)
        {
            reliquary.signals.batchSources.map.emplace(
                TypeFor<SignalT>().name,
                std::make_unique<BatchSource<SignalT>>());
        };
        signalList.emplace_back(type, factory);

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
        const auto type = TypeFor<RelicT>();

        if (IsGlobalRelicRegistered<RelicT>())
            throw AlreadyRegistered("global relic", type, typeid(RelicT));

        const auto factory =
            [args = std::make_tuple(std::forward<InitializeArgs>(initializeArgs) ...)]
        (Reliquary& reliquary)
        {
            auto execution = [&reliquary](auto&& ... initializeArgs)
            {
                const auto type = TypeFor<RelicT>();
                const auto id = reliquary.relics.AdvanceID();
                auto emplaced = reliquary.relics.globalMap.emplace(
                    type.name,
                    ReliquaryRelics::StoredGlobal
                    {
                        std::make_shared<RelicT>(),
                        id
                    }
                );
                auto relic = reinterpret_cast<RelicT*>(emplaced.first->second.storage.get());
                relic->id = id;
                relic->owner = &reliquary;
                reliquary.relics.SetupNewInternals(
                    id,
                    OpennessFor<RelicT>(),
                    LocalityFor<RelicT>(),
                    HasScribe<RelicT>(),
                    Arca::Type(type.name),
                    relic);
                reliquary.relics.SatisfyStructure(id, StructureFrom<shards_for_t<RelicT>>());
                PostConstruct(*relic);
                Initialize(*relic, std::forward<decltype(initializeArgs)>(initializeArgs)...);

                if (HasScribe<RelicT>())
                {
                    reliquary.relics.globalSerializers.push_back(
                        KnownPolymorphicSerializer
                        {
                            type.name,
                            [relic](Reliquary&, ::Inscription::BinaryArchive& archive)
                            {
                                archive(*relic);
                            },
                            [](::Inscription::BinaryArchive& archive)
                            {
                                return ::Inscription::InputTypesFor<RelicT>(archive);
                            }
                        });

                    reliquary.relics.globalConstructList.push_back(
                        [relic](Reliquary& reliquary)
                        {
                            relic->owner = &reliquary;
                            PostConstruct(*relic);
                        });
                }
            };
            return std::apply(execution, args);
        };
        globalRelicList.emplace_back(type.name, factory);
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
    bool ReliquaryOrigin::IsGlobalRelicAliasRegistered() const
    {
        const std::type_index type(typeid(T));
        const auto found = globalRelicAliasInitializerMap.find(type);
        return found != globalRelicAliasInitializerMap.end();
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

    template<class CuratorT, class CuratorProvider, class... Args>
    void ReliquaryOrigin::CuratorCommon(Args&& ... args)
    {
        const auto type = TypeFor<CuratorT>();

        if (IsCuratorRegistered<CuratorT>())
            throw AlreadyRegistered("curator", type, typeid(CuratorT));

        curatorProviders.emplace(type.name, std::make_unique<CuratorProvider>(std::forward<Args>(args)...));
        if (HasScribe<CuratorT>())
        {
            const auto curatorSerializationTypesFactory = [](Reliquary& reliquary)
            {
                const auto type = TypeFor<CuratorT>();
                reliquary.curators.serializers.push_back(
                    KnownPolymorphicSerializer
                    {
                        type.name,
                        [](Reliquary& reliquary, ::Inscription::BinaryArchive& archive)
                        {
                            auto& curator = reliquary.Find<CuratorT>();
                            archive(curator);
                        },
                        [](::Inscription::BinaryArchive& archive)
                        {
                            return ::Inscription::InputTypesFor<CuratorT>(archive);
                        }
                    });
            };

            curatorSerializationTypesFactoryList.push_back(curatorSerializationTypesFactory);
        }
    }

    template<class CuratorT>
    bool ReliquaryOrigin::IsCuratorRegistered() const
    {
        auto type = TypeFor<CuratorT>();
        return curatorProviders.find(type.name) != curatorProviders.end();
    }

    template<class SignalT>
    bool ReliquaryOrigin::IsSignalRegistered() const
    {
        const auto found = std::find_if(
            signalList.begin(),
            signalList.end(),
            [](const SignalConstructor& constructor)
            {
                return constructor.type == typeid(SignalT);
            });
        return found != signalList.end();
    }
}