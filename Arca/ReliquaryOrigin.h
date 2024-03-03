#pragma once

#include <vector>

#include "Reliquary.h"

#include "Curator.h"
#include "CuratorProvider.h"
#include "CuratorPipeline.h"

namespace Arca
{
    class ReliquaryOrigin
    {
    public:
        ReliquaryOrigin() = default;
        ReliquaryOrigin(const ReliquaryOrigin& arg);
        ReliquaryOrigin(ReliquaryOrigin&& arg) = default;
        ReliquaryOrigin& operator=(const ReliquaryOrigin& arg);
        ReliquaryOrigin& operator=(ReliquaryOrigin&& arg) = default;
    public:
        [[nodiscard]] Reliquary Actualize() const;
    public:
        template<class RelicT>
        ReliquaryOrigin& Relic();
        template<class RelicT, class... CreationArgs>
        ReliquaryOrigin& StaticRelic(CreationArgs&& ... creationArgs);
        ReliquaryOrigin& RelicStructure(const std::string& name, const RelicStructure& structure);
    public:
        template<class ShardT>
        ReliquaryOrigin& Shard();
    public:
        template<class CuratorT, class... Args>
        ReliquaryOrigin& Curator(Args&& ... args);
        template<class AsT, class ProvidedT, typename std::enable_if<std::is_base_of_v<ProvidedT, Arca::Curator>, int>::type = 0>
        ReliquaryOrigin& Curator(ProvidedT* use);
        ReliquaryOrigin& CuratorPipeline(const CuratorPipeline& pipeline);
    public:
        template<class SignalT>
        ReliquaryOrigin& Signal();
    private:
        struct TypeConstructor
        {
            TypeHandle typeHandle;
            std::function<void(Reliquary&)> factory;
        };
        using TypeConstructorList = std::vector<TypeConstructor>;
    private:
        TypeConstructorList relicList;

        template<class RelicT>
        [[nodiscard]] bool IsRelicRegistered() const;
    private:
        TypeConstructorList staticRelicList;

        template<class RelicT>
        [[nodiscard]] bool IsStaticRelicRegistered() const;
    private:
        using TypedRelicInitializer = void(*)(Reliquary&);
        using TypedRelicInitializerList = std::vector<TypedRelicInitializer>;
        TypedRelicInitializerList typedRelicInitializerList;
    private:
        using NamedRelicStructure = Reliquary::NamedRelicStructure;
        using NamedRelicStructureList = std::vector<NamedRelicStructure>;
        NamedRelicStructureList namedRelicStructureList;
    private:
        TypeConstructorList shardList;

        template<class ShardT>
        [[nodiscard]] bool IsShardRegistered() const;
    private:
        using CuratorProviderPtr = std::unique_ptr<CuratorProviderBase>;
        using CuratorProviderMap = std::unordered_map<TypeHandle, CuratorProviderPtr>;
        CuratorProviderMap curatorProviders;

        Arca::CuratorPipeline curatorPipeline;

        using CuratorSerializationTypeHandlesFactory = void(*)(Reliquary&);
        using CuratorSerializationTypeHandlesFactoryList = std::vector<CuratorSerializationTypeHandlesFactory>;
        CuratorSerializationTypeHandlesFactoryList curatorSerializationTypeHandlesFactoryList;

        void PushAllCuratorsTo(Reliquary& reliquary) const;

        template<class Curator, class CuratorProvider, class... Args>
        void CuratorCommon(Args&& ... args);
        template<class Curator>
        [[nodiscard]] bool IsCuratorRegistered() const;

        void PushCuratorPipeline(Reliquary& reliquary) const;
    private:
        struct SignalConstructor
        {
            std::type_index type;
            void(*factory)(Reliquary&);
        };
        std::vector<SignalConstructor> signalList;

        template<class SignalT>
        [[nodiscard]] bool IsSignalRegistered() const;
    };

    template<class RelicT>
    ReliquaryOrigin& ReliquaryOrigin::Relic()
    {
        if (IsRelicRegistered<RelicT>())
            throw AlreadyRegistered();

        const auto typeHandle = RelicTraits<RelicT>::typeHandle;
        const auto factory = [](Reliquary& reliquary)
        {
            const auto typeHandle = RelicTraits<RelicT>::typeHandle;
            reliquary.relicBatchSources.emplace(typeHandle, std::make_unique<RelicBatchSource<RelicT>>());
            reliquary.relicSerializerMap.emplace(
                typeHandle,
                Reliquary::KnownPolymorphicSerializer
                {
                    [](void* relicBatchSource, Reliquary& reliquary, ::Inscription::BinaryArchive& archive)
                    {
                        auto castedRelicBatchSource = static_cast<RelicBatchSource<RelicT>*>(relicBatchSource);
                        archive(*castedRelicBatchSource);

                        if (archive.IsInput())
                        {
                            for(auto& loop : *castedRelicBatchSource)
                            {
                                loop.Initialize(reliquary);

                                auto metadata = reliquary.RelicMetadataFor(loop.ID());
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
        relicList.push_back({ typeHandle, factory });

        typedRelicInitializerList.push_back([](Reliquary& reliquary)
            {
                auto& relicBatchSource = reliquary.RequiredRelicBatchSource<RelicT>();
                for (auto& loop : relicBatchSource)
                    loop.Initialize(reliquary);
            });

        Signal<RelicCreated<RelicT>>();
        Signal<BeforeRelicDestroyed<RelicT>>();

        return *this;
    }

    template<class RelicT, class... CreationArgs>
    ReliquaryOrigin& ReliquaryOrigin::StaticRelic(CreationArgs&& ... creationArgs)
    {
        if (IsStaticRelicRegistered<RelicT>())
            throw AlreadyRegistered();

        const auto typeHandle = RelicTraits<RelicT>::typeHandle;
        const auto factory = [args = std::make_tuple(std::forward<CreationArgs>(creationArgs) ...)](Reliquary& reliquary)
        {
            return std::apply([&reliquary](auto&& ... creationArgs)
                {
                    const auto typeHandle = RelicTraits<RelicT>::typeHandle;
                    const auto id = reliquary.NextRelicID();
                    auto relic = RelicT(std::forward<CreationArgs>(creationArgs)...);
                    relic.id = id;
                    auto emplaced = reliquary.staticRelicMap.emplace(typeHandle, std::move(relic)).first->second;
                    auto castedEmplaced = std::any_cast<RelicT>(&emplaced);
                    reliquary.SetupNewRelicInternals(id, RelicDynamism::Static, typeHandle, castedEmplaced);
                    reliquary.SatisfyRelicStructure(relic.Structure(), id);

                    reliquary.staticRelicSerializerMap.emplace(
                        typeHandle,
                        Reliquary::KnownPolymorphicSerializer
                        {
                            [](void* staticRelicStorage, Reliquary&, ::Inscription::BinaryArchive& archive)
                            {
                                auto& staticRelicAsAny = *static_cast<std::any*>(staticRelicStorage);
                                auto staticRelic = std::any_cast<RelicT>(staticRelicAsAny);
                                archive(staticRelic);
                            },
                            [](::Inscription::BinaryArchive& archive)
                            {
                                return ::Inscription::InputTypeHandlesFor<RelicT>(archive);
                            }
                        });
                }, std::move(args));
        };
        staticRelicList.push_back({ typeHandle, factory });

        typedRelicInitializerList.push_back([](Reliquary& reliquary)
            {
                auto relic = reliquary.StaticRelic<RelicT>();
                relic->Initialize(reliquary);
            });

        return *this;
    }

    template<class ShardT>
    ReliquaryOrigin& ReliquaryOrigin::Shard()
    {
        if (IsShardRegistered<ShardT>())
            throw AlreadyRegistered();

        const auto factory = [](Reliquary& reliquary)
        {
            const auto typeHandle = ShardTraits<ShardT>::typeHandle;
            reliquary.shardBatchSources.emplace(typeHandle, std::make_unique<ShardBatchSource<ShardT>>());
            reliquary.shardFactoryMap.emplace(
                typeHandle,
                [](Reliquary& reliquary, RelicID id)
                {
                    auto found = reliquary.FindShardBatchSource<ShardT>();
                    auto added = found->Add(id);
                    reliquary.SignalShardCreation(*added);
                });
            reliquary.shardSerializerMap.emplace(
                typeHandle,
                Reliquary::KnownPolymorphicSerializer
                {
                    [](void* shardBatchSource, Reliquary&, ::Inscription::BinaryArchive& archive)
                    {
                        auto castedShardBatchSource = static_cast<ShardBatchSource<ShardT>*>(shardBatchSource);
                        archive(*castedShardBatchSource);
                    },
                    [](::Inscription::BinaryArchive& archive)
                    {
                        return ::Inscription::InputTypeHandlesFor<ShardT>(archive);
                    }
                });
        };

        const auto typeHandle = ShardTraits<ShardT>::typeHandle;
        shardList.push_back({ typeHandle, factory });

        Signal<ShardCreated<ShardT>>();
        Signal<BeforeShardDestroyed<ShardT>>();

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

        CuratorCommon<AsT, ExternalCuratorProvider>(use, CuratorTraits<ProvidedT>::typeHandle);

        return *this;
    }

    template<class SignalT>
    ReliquaryOrigin& ReliquaryOrigin::Signal()
    {
        if (IsSignalRegistered<SignalT>())
            throw AlreadyRegistered();

        const auto type = std::type_index(typeid(SignalT));
        const auto factory = [](Reliquary& reliquary)
        {
            reliquary.signalBatchSources.emplace(
                reliquary.KeyForSignalBatchSource<SignalT>(),
                std::make_unique<SignalBatchSource<SignalT>>());
        };
        signalList.push_back({ type, factory });

        return *this;
    }

    template<class RelicT>
    [[nodiscard]] bool ReliquaryOrigin::IsRelicRegistered() const
    {
        const auto typeHandle = RelicTraits<RelicT>::typeHandle;
        const auto found = std::find_if(
            relicList.begin(),
            relicList.end(),
            [typeHandle](const TypeConstructor& constructor)
            {
                return constructor.typeHandle == typeHandle;
            });
        return found != relicList.end();
    }

    template<class RelicT>
    bool ReliquaryOrigin::IsStaticRelicRegistered() const
    {
        const auto typeHandle = RelicTraits<RelicT>::typeHandle;
        const auto found = std::find_if(
            staticRelicList.begin(),
            staticRelicList.end(),
            [typeHandle](const TypeConstructor& constructor)
            {
                return constructor.typeHandle == typeHandle;
            });
        return found != staticRelicList.end();
    }

    template<class ShardT>
    bool ReliquaryOrigin::IsShardRegistered() const
    {
        const auto typeHandle = ShardTraits<ShardT>::typeHandle;
        const auto found = std::find_if(
            shardList.begin(),
            shardList.end(),
            [typeHandle](const TypeConstructor& constructor)
            {
                return constructor.typeHandle == typeHandle;
            });
        return found != shardList.end();
    }

    template<class CuratorT, class CuratorProvider, class... Args>
    void ReliquaryOrigin::CuratorCommon(Args&& ... args)
    {
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(CuratorT);

        if (IsCuratorRegistered<CuratorT>())
            throw AlreadyRegistered();

        auto typeHandle = CuratorTraits<CuratorT>::typeHandle;
        curatorProviders.emplace(typeHandle, std::make_unique<CuratorProvider>(std::forward<Args>(args)...));
        const auto curatorSerializationTypeHandlesFactory = [](Reliquary& reliquary)
        {
            auto typeHandle = CuratorTraits<CuratorT>::typeHandle;
            reliquary.curatorSerializerMap.emplace(
                typeHandle,
                Reliquary::KnownPolymorphicSerializer
                {
                    [](void* curator, Reliquary&, ::Inscription::BinaryArchive& archive)
                    {
                        auto castedCurator = static_cast<CuratorT*>(curator);
                        archive(*castedCurator);
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
        auto typeHandle = CuratorTraits<CuratorT>::typeHandle;
        return curatorProviders.find(typeHandle) != curatorProviders.end();
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