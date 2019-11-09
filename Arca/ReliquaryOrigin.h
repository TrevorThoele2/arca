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
        template<class RelicT>
        ReliquaryOrigin& StaticRelic();
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
        struct TypedInitializer
        {
            TypeHandle typeHandle;
            void(*factory)(Reliquary&);
        };
        using TypedInitializerList = std::vector<TypedInitializer>;
    private:
        TypedInitializerList relicList;

        template<class RelicT>
        [[nodiscard]] bool IsRelicRegistered() const;
    private:
        TypedInitializerList staticRelicList;

        template<class RelicT>
        [[nodiscard]] bool IsStaticRelicRegistered() const;
    private:
        using NamedRelicStructure = Reliquary::NamedRelicStructure;
        using NamedRelicStructureList = std::vector<NamedRelicStructure>;
        NamedRelicStructureList namedRelicStructureList;
    private:
        TypedInitializerList shardList;

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
        struct SignalInitializer
        {
            std::type_index type;
            void(*factory)(Reliquary&);
        };
        std::vector<SignalInitializer> signalList;

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
                    [](void* relicBatchSource, ::Inscription::BinaryArchive& archive)
                    {
                        auto castedRelicBatchSource = static_cast<RelicBatchSource<RelicT>*>(relicBatchSource);
                        archive(*castedRelicBatchSource);
                    },
                    [](::Inscription::BinaryArchive& archive)
                    {
                        return ::Inscription::InputTypeHandlesFor<RelicT>(archive);
                    }
                });
        };
        relicList.push_back({ typeHandle, factory });

        return *this;
    }

    template<class RelicT>
    ReliquaryOrigin& ReliquaryOrigin::StaticRelic()
    {
        if (IsStaticRelicRegistered<RelicT>())
            throw AlreadyRegistered();

        const auto typeHandle = RelicTraits<RelicT>::typeHandle;
        const auto factory = [](Reliquary& reliquary)
        {
            const auto typeHandle = RelicTraits<RelicT>::typeHandle;
            const auto id = reliquary.SetupNewRelicInternals(RelicDynamism::Static, typeHandle);
            reliquary.SatisfyRelicStructure(RelicT::structure, id);
            reliquary.staticRelicIDMap.emplace(typeHandle, id);
        };
        staticRelicList.push_back({ typeHandle, factory });

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
                    reliquary.SignalCreation(*added);
                });
            reliquary.shardSerializerMap.emplace(
                typeHandle,
                Reliquary::KnownPolymorphicSerializer
                {
                    [](void* shardBatchSource, ::Inscription::BinaryArchive& archive)
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
            [typeHandle](const TypedInitializer& initializer)
            {
                return initializer.typeHandle == typeHandle;
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
            [typeHandle](const TypedInitializer& initializer)
            {
                return initializer.typeHandle == typeHandle;
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
            [typeHandle](const TypedInitializer& initializer)
            {
                return initializer.typeHandle == typeHandle;
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
                    [](void* curator, ::Inscription::BinaryArchive& archive)
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
            [](const SignalInitializer& initializer)
            {
                return initializer.type == typeid(SignalT);
            });
        return found != signalList.end();
    }
}