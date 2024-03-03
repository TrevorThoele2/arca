#pragma once

#include <vector>

#include "Reliquary.h"

#include "Pipeline.h"

#include "Curator.h"
#include "CuratorProvider.h"

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
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        ReliquaryOrigin& Relic();
        template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        ReliquaryOrigin& GlobalRelic(CreationArgs&& ... creationArgs);
        ReliquaryOrigin& RelicStructure(const std::string& name, const RelicStructure& structure);
    public:
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ReliquaryOrigin& Shard();
    public:
        template<class CuratorT, class... Args>
        ReliquaryOrigin& Curator(Args&& ... args);
        template<class AsT, class ProvidedT, typename std::enable_if<std::is_base_of_v<ProvidedT, Arca::Curator>, int>::type = 0>
        ReliquaryOrigin& Curator(ProvidedT* use);
        ReliquaryOrigin& CuratorPipeline(const Pipeline& pipeline);
        ReliquaryOrigin& CuratorPipeline(const Pipeline& initialization, const Pipeline& work);
    public:
        template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int> = 0>
        ReliquaryOrigin& Signal();
    private:
        struct TypeConstructor
        {
            TypeHandleName typeHandleName;
            std::function<void(Reliquary&)> factory;

            TypeConstructor(TypeHandleName typeHandleName, std::function<void(Reliquary&)>&& factory);
        };
        using TypeConstructorList = std::vector<TypeConstructor>;
    private:
        TypeConstructorList relicList;

        template<class RelicT>
        [[nodiscard]] bool IsRelicRegistered() const;
    private:
        TypeConstructorList globalRelicList;

        template<class RelicT>
        [[nodiscard]] bool IsGlobalRelicRegistered() const;
    private:
        using TypedRelicInitializer = void(*)(Reliquary&);
        using TypedRelicInitializerList = std::vector<TypedRelicInitializer>;
        TypedRelicInitializerList typedRelicInitializerList;
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
        using CuratorProviderMap = std::unordered_map<TypeHandleName, CuratorProviderPtr>;
        CuratorProviderMap curatorProviders;

        Pipeline curatorInitializationPipeline;
        Pipeline curatorWorkPipeline;

        using CuratorSerializationTypeHandlesFactory = void(*)(Reliquary&);
        using CuratorSerializationTypeHandlesFactoryList = std::vector<CuratorSerializationTypeHandlesFactory>;
        CuratorSerializationTypeHandlesFactoryList curatorSerializationTypeHandlesFactoryList;

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

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Relic()
    {
        const auto typeHandle = TypeHandleFor<RelicT>();

        if (IsRelicRegistered<RelicT>())
            throw AlreadyRegistered("relic", typeHandle, typeid(RelicT));

        const auto factory = [](Reliquary& reliquary)
        {
            const auto typeHandle = TypeHandleFor<RelicT>();
            reliquary.relics.batchSources.map.emplace(
                typeHandle.name,
                std::make_unique<BatchSource<RelicT>>(reliquary));
            reliquary.relics.serializers.push_back(
                KnownPolymorphicSerializer
                {
                    typeHandle.name,
                    [](Reliquary& reliquary, ::Inscription::BinaryArchive& archive)
                    {
                        auto batchSource = reliquary.relics.batchSources.Find<RelicT>();
                        archive(*batchSource);

                        if (archive.IsInput())
                        {
                            for(auto& loop : *batchSource)
                            {
                                loop.Initialize(reliquary);

                                auto metadata = reliquary.relics.MetadataFor(loop.ID());
                                metadata->storage = &loop;
                            }
                        }
                    },
                    [](::Inscription::BinaryArchive& archive)
                    {
                        return ::Inscription::InputTypeHandlesFor<RelicT>(archive);
                    }
                });
        };
        relicList.emplace_back(typeHandle.name, factory);

        typedRelicInitializerList.push_back([](Reliquary& reliquary)
            {
                auto& relicBatchSource = reliquary.relics.batchSources.Required<RelicT>();
                for (auto& loop : relicBatchSource)
                    loop.Initialize(reliquary);
            });

        return *this;
    }

    template<class RelicT, class... CreationArgs, std::enable_if_t<is_relic_v<RelicT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::GlobalRelic(CreationArgs&& ... creationArgs)
    {
        const auto typeHandle = TypeHandleFor<RelicT>();

        if (IsGlobalRelicRegistered<RelicT>())
            throw AlreadyRegistered("global relic", typeHandle, typeid(RelicT));

        const auto factory = [args = std::make_tuple(std::forward<CreationArgs>(creationArgs) ...)](Reliquary& reliquary)
        {
            return std::apply([&reliquary](auto&& ... creationArgs)
                {
                    const auto typeHandle = TypeHandleFor<RelicT>();
                    const auto id = reliquary.relics.AdvanceID();
                    auto relic = RelicT(std::forward<CreationArgs>(creationArgs)...);
                    relic.id = id;
                    auto emplaced = reliquary.relics.globalMap.emplace(
                        typeHandle.name,
                        std::move(relic)
                    )
                        .first->second;
                    auto castedEmplaced = std::any_cast<RelicT>(&emplaced);
                    reliquary.relics.SetupNewInternals(id, RelicOpenness::Global, typeHandle.name, castedEmplaced);
                    reliquary.relics.SatisfyStructure(id, StructureFrom<shards_for_t<RelicT>>());

                    reliquary.relics.globalSerializers.push_back(
                        KnownPolymorphicSerializer
                        {
                            typeHandle.name,
                            [](Reliquary& reliquary, ::Inscription::BinaryArchive& archive)
                            {
                                auto relic = reliquary.Find<Global<RelicT>>();
                                archive(*relic);
                            },
                            [](::Inscription::BinaryArchive& archive)
                            {
                                return ::Inscription::InputTypeHandlesFor<RelicT>(archive);
                            }
                        });
                }, args);
        };
        globalRelicList.emplace_back(typeHandle.name, factory);

        typedRelicInitializerList.push_back([](Reliquary& reliquary)
            {
                auto relic = reliquary.Find<Global<RelicT>>();
                relic->Initialize(reliquary);
            });

        return *this;
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Shard()
    {
        const auto typeHandle = TypeHandleFor<ShardT>();

        if (IsShardRegistered<ShardT>())
            throw AlreadyRegistered("shard", typeHandle, typeid(ShardT));

        const auto factory = [typeHandle](Reliquary& reliquary)
        {
            reliquary.shards.batchSources.map.emplace(
                typeHandle.name,
                std::make_unique<BatchSource<ShardT>>());
            reliquary.shards.batchSources.constMap.emplace(
                typeHandle.name,
                std::make_unique<BatchSource<const ShardT>>());

            reliquary.shards.factoryMap.emplace(
                typeHandle.name,
                [](Reliquary& reliquary, RelicID id, bool isConst)
                {
                    const auto creator = [id, &reliquary](auto found)
                    {
                        auto added = found->Add(id);
                        reliquary.shards.AttemptAddToEitherBatches(id, *added);
                        reliquary.Raise<Created>(Handle(id, reliquary));
                    };

                    isConst
                        ? creator(reliquary.shards.batchSources.Find<const ShardT>())
                        : creator(reliquary.shards.batchSources.Find<ShardT>());
                });
            reliquary.shards.serializers.push_back(
                KnownPolymorphicSerializer
                {
                    typeHandle.name,
                    [](Reliquary& reliquary, ::Inscription::BinaryArchive& archive)
                    {
                        auto batchSource = reliquary.shards.batchSources.Find<ShardT>();
                        archive(*batchSource);

                        auto constBatchSource = reliquary.shards.batchSources.Find<const ShardT>();
                        archive(*constBatchSource);
                    },
                    [](::Inscription::BinaryArchive& archive)
                    {
                        return ::Inscription::InputTypeHandlesFor<ShardT>(archive);
                    }
                });
        };

        shardList.emplace_back(typeHandle.name, std::move(factory));

        return *this;
    }

    template<class CuratorT, class... Args>
    ReliquaryOrigin& ReliquaryOrigin::Curator(Args&& ... args)
    {
        CuratorCommon<CuratorT, CuratorProvider<CuratorT, Args...>>(std::make_tuple(std::forward<Args>(args)...));

        return *this;
    }

    template<class AsT, class ProvidedT, typename std::enable_if<std::is_base_of_v<ProvidedT, Curator>, int>::type>
    ReliquaryOrigin& ReliquaryOrigin::Curator(ProvidedT* use)
    {
        if (!dynamic_cast<AsT>(use))
            throw IncorrectRegisteredCuratorType();

        CuratorCommon<AsT, ExternalCuratorProvider>(use, TypeHandleFor<ProvidedT>());

        return *this;
    }

    template<class SignalT, std::enable_if_t<is_signal_v<SignalT>, int>>
    ReliquaryOrigin& ReliquaryOrigin::Signal()
    {
        const auto type = std::type_index(typeid(SignalT));

        if (IsSignalRegistered<SignalT>())
            throw AlreadyRegistered("signal", { type.name() });

        const auto factory = [](Reliquary& reliquary)
        {
            reliquary.signals.batchSources.map.emplace(
                TypeHandleFor<SignalT>().name,
                std::make_unique<BatchSource<SignalT>>());
        };
        signalList.emplace_back(type, factory);

        return *this;
    }

    template<class RelicT>
    [[nodiscard]] bool ReliquaryOrigin::IsRelicRegistered() const
    {
        const auto typeHandle = TypeHandleFor<RelicT>();
        const auto found = std::find_if(
            relicList.begin(),
            relicList.end(),
            [typeHandle](const TypeConstructor& constructor)
            {
                return constructor.typeHandleName == typeHandle.name;
            });
        return found != relicList.end();
    }

    template<class RelicT>
    bool ReliquaryOrigin::IsGlobalRelicRegistered() const
    {
        const auto typeHandle = TypeHandleFor<RelicT>();
        const auto found = std::find_if(
            globalRelicList.begin(),
            globalRelicList.end(),
            [typeHandle](const TypeConstructor& constructor)
            {
                return constructor.typeHandleName == typeHandle.name;
            });
        return found != globalRelicList.end();
    }

    template<class ShardT>
    bool ReliquaryOrigin::IsShardRegistered() const
    {
        const auto typeHandle = TypeHandleFor<ShardT>();
        const auto found = std::find_if(
            shardList.begin(),
            shardList.end(),
            [typeHandle](const TypeConstructor& constructor)
            {
                return constructor.typeHandleName == typeHandle.name;
            });
        return found != shardList.end();
    }

    template<class CuratorT, class CuratorProvider, class... Args>
    void ReliquaryOrigin::CuratorCommon(Args&& ... args)
    {
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(CuratorT);

        const auto typeHandle = TypeHandleFor<CuratorT>();

        if (IsCuratorRegistered<CuratorT>())
            throw AlreadyRegistered("curator", typeHandle, typeid(CuratorT));

        curatorProviders.emplace(typeHandle.name, std::make_unique<CuratorProvider>(std::forward<Args>(args)...));
        const auto curatorSerializationTypeHandlesFactory = [](Reliquary& reliquary)
        {
            const auto typeHandle = TypeHandleFor<CuratorT>();
            reliquary.curators.serializers.push_back(
                KnownPolymorphicSerializer
                {
                    typeHandle.name,
                    [](Reliquary& reliquary, ::Inscription::BinaryArchive& archive)
                    {
                        auto curator = reliquary.Find<CuratorT>();
                        archive(*curator);
                    },
                    [](::Inscription::BinaryArchive& archive)
                    {
                        return ::Inscription::InputTypeHandlesFor<CuratorT>(archive);
                    }
                });
        };

        curatorSerializationTypeHandlesFactoryList.push_back(curatorSerializationTypeHandlesFactory);
    }

    template<class CuratorT>
    bool ReliquaryOrigin::IsCuratorRegistered() const
    {
        auto typeHandle = TypeHandleFor<CuratorT>();
        return curatorProviders.find(typeHandle.name) != curatorProviders.end();
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