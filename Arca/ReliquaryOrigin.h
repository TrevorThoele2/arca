#pragma once

#include <vector>

#include "Reliquary.h"

#include "Curator.h"
#include "CuratorProvider.h"
#include "CuratorLayout.h"

namespace Arca
{
    class ReliquaryOrigin
    {
    public:
        [[nodiscard]] Reliquary Actualize() const;
    public:
        template<class VesselT>
        ReliquaryOrigin& StaticVessel();
    public:
        template<class RelicT>
        ReliquaryOrigin& Relic();
    public:
        template<class CuratorT, class... Args>
        ReliquaryOrigin& Curator(Args&& ... args);
        template<class AsT, class ProvidedT, typename std::enable_if<std::is_base_of_v<ProvidedT, Arca::Curator>, int>::type = 0>
        ReliquaryOrigin& Curator(ProvidedT* use);
        ReliquaryOrigin& CuratorLayout(const CuratorLayout& layout);
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
        TypedInitializerList staticVesselList;

        template<class VesselT>
        [[nodiscard]] bool IsStaticVesselRegistered() const;
    private:
        TypedInitializerList relicList;

        template<class RelicT>
        [[nodiscard]] bool IsRelicRegistered() const;
    private:
        using CuratorProviderPtr = std::unique_ptr<CuratorProviderBase>;
        using CuratorProviderMap = std::unordered_map<TypeHandle, CuratorProviderPtr>;
        CuratorProviderMap curatorProviders;

        using CuratorLayoutList = std::vector<Arca::CuratorLayout>;
        CuratorLayoutList curatorLayoutList;

        using CuratorSerializationTypeHandlesFactory = void(*)(Reliquary&);
        using CuratorSerializationTypeHandlesFactoryList = std::vector<CuratorSerializationTypeHandlesFactory>;
        CuratorSerializationTypeHandlesFactoryList curatorSerializationTypeHandlesFactoryList;

        void PushAllCuratorsTo(Reliquary& reliquary) const;

        template<class Curator, class CuratorProvider, class... Args>
        void CuratorCommon(Args&& ... args);
        template<class Curator>
        [[nodiscard]] bool IsCuratorRegistered() const;
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

    template<class VesselT>
    ReliquaryOrigin& ReliquaryOrigin::StaticVessel()
    {
        if (IsStaticVesselRegistered<VesselT>())
            throw AlreadyRegistered();

        const auto typeHandle = VesselTraits<VesselT>::typeHandle;
        const auto factory = [](Reliquary& reliquary)
        {
            const auto typeHandle = VesselTraits<VesselT>::typeHandle;
            const auto id = reliquary.SetupNewVesselInternals(VesselDynamism::Static, typeHandle);
            reliquary.SatisfyVesselStructure(VesselT::structure, id);
            reliquary.staticVesselIDMap.emplace(typeHandle, id);
        };
        staticVesselList.push_back({ typeHandle, factory });

        return *this;
    }

    template<class RelicT>
    ReliquaryOrigin& ReliquaryOrigin::Relic()
    {
        if (IsRelicRegistered<RelicT>())
            throw AlreadyRegistered();

        const auto factory = [](Reliquary& reliquary)
        {
            const auto typeHandle = RelicTraits<RelicT>::typeHandle;
            reliquary.relicBatchSources.emplace(typeHandle, std::make_unique<RelicBatchSource<RelicT>>());
            reliquary.relicFactoryMap.emplace(
                typeHandle,
                [](Reliquary& reliquary, VesselID id)
                {
                    auto found = reliquary.FindRelicBatchSource<RelicT>();
                    auto added = found->Add(id);
                    reliquary.SignalCreation(*added);
                });
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

        const auto typeHandle = RelicTraits<RelicT>::typeHandle;
        relicList.push_back({ typeHandle, factory });

        Signal<RelicCreated<RelicT>>();
        Signal<BeforeRelicDestroyed<RelicT>>();

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

    template<class SignalT>
    bool ReliquaryOrigin::IsStaticVesselRegistered() const
    {
        const auto type = std::type_index(typeid(SignalT));
        const auto found = std::find_if(
            signalList.begin(),
            signalList.end(),
            [type](const SignalInitializer& initializer)
            {
                return initializer.type == type;
            });
        return found != signalList.end();
    }

    template<class RelicT>
    bool ReliquaryOrigin::IsRelicRegistered() const
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