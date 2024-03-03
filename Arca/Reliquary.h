#pragma once

#include <unordered_map>
#include <optional>

#include "ReliquaryException.h"

#include "BasicRelicFactory.h"
#include "RelicBatch.h"
#include "RelicBatchSource.h"
#include "AbstractRelicBatch.h"
#include "AbstractRelicBatchSource.h"
#include "ProcessedRelicTraits.h"

#include "SignalBatch.h"
#include "SignalBatchSource.h"
#include "RelicCreated.h"
#include "BeforeRelicDestroyed.h"

#include "RelicTypeGraph.h"

#include "Curator.h"
#include "CuratorProvider.h"
#include "CuratorLayout.h"
#include "CuratorHandle.h"
#include "ProcessedCuratorTraits.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary
    {
    public:
        using SizeT = size_t;
    public:
        Reliquary() = default;
        Reliquary(const Reliquary& arg) = delete;
        Reliquary(Reliquary&& arg) noexcept;
        ~Reliquary() = default;
        Reliquary& operator=(const Reliquary& arg) = delete;
        Reliquary& operator=(Reliquary&& arg) noexcept;

        void Initialize(const std::function<void(Reliquary&)>& afterCuratorsCreated = [](Reliquary&) {});
        void Work();
    public:
        template<
            class RelicT,
            class... Args,
            std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic && !std::is_abstract_v<RelicT>, int> = 0
        > Ref<RelicT> CreateRelic(Args&& ... args);

        template<class RelicT, std::enable_if_t<ProcessedRelicTraits<RelicT>::isStatic, int> = 0>
        [[nodiscard]] Ref<RelicT> StaticRelic();

        template<class RelicT, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int> = 0>
        void DestroyRelic(Ref<RelicT>& destroy);

        [[nodiscard]] SizeT RelicCount() const;

        template<class RelicT, std::enable_if_t<!std::is_abstract_v<RelicT>, int> = 0>
        [[nodiscard]] RelicBatch<RelicT> StartRelicBatch();
        template<class RelicT, std::enable_if_t<std::is_abstract_v<RelicT>, int> = 0>
        [[nodiscard]] AbstractRelicBatch<RelicT> StartRelicBatch();

        [[nodiscard]] RelicTypeGraph RelicTypeGraph() const;

        template<class RelicT, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int> = 0>
        void RegisterRelicType();
        template<class RelicT, class... Args, std::enable_if_t<ProcessedRelicTraits<RelicT>::isStatic, int> = 0>
        void RegisterRelicType(Args&& ... args);
    public:
        template<class CuratorT>
        [[nodiscard]] CuratorT* FindCurator();
        template<class CuratorT>
        [[nodiscard]] const CuratorT* FindCurator() const;

        [[nodiscard]] std::vector<CuratorTypeDescription> CuratorTypeDescriptions() const;

        template<class T, class... Args>
        void RegisterCuratorType(Args&& ... args);
        template<class T, class CuratorT, typename std::enable_if<std::is_base_of_v<CuratorT, Curator>, int>::type = 0>
        void RegisterCuratorType(CuratorT* use);
        void RegisterCuratorLayout(const CuratorLayout& layout);
    public:
        template<class SignalT>
        void RaiseSignal(const SignalT& signal);

        template<class SignalT>
        SignalBatch<SignalT> StartSignalBatch();

        template<class SignalT>
        void RegisterSignalType();
    private:
        bool isInitialized = false;
    private:
        const RelicID staticRelicID = 1;

        using StaticRelicInitializer = void(*)(Reliquary&);
        using StaticRelicInitializerList = std::vector<StaticRelicInitializer>;
        StaticRelicInitializerList staticRelicInitializerList;

        Arca::RelicTypeGraph relicTypeGraph;

        template<class RelicT, class... Args>
        Ref<RelicT> CreateRelicCommon(Args&& ... args);

        template<class RelicT>
        void NotifyAddedAbstractRelicBatchSources(Ref<RelicT>& relic);
        template<class RelicT>
        void NotifyDestroyedAbstractRelicBatchSources(Ref<RelicT>& relic);

        template<class RelicT>
        void SignalCreation(const Ref<RelicT>& relic);
        template<class RelicT>
        void SignalDestruction(const Ref<RelicT>& relic);

        template<class RelicT, class... Args, std::enable_if_t<ProcessedRelicTraits<RelicT>::isStatic, int> = 0>
        void CreateStaticRelicInitializer(Args&& ... args);
        void CreateAllStaticRelics();

        template<class RelicT>
        void RegisterRelicTypeCommon();
        template<class RelicT>
        [[nodiscard]] bool IsRelicRegistered() const;

        template<class RelicT>
        static TypeHandle TypeHandleForRelic();
        template<class RelicT>
        static RelicTypeDescription TypeDescriptionForRelic();
    private:
        using RelicBatchSourcePtr = std::unique_ptr<RelicBatchSourceBase>;
        using RelicBatchSourceList = std::unordered_map<TypeHandle, RelicBatchSourcePtr>;
        RelicBatchSourceList relicBatchSources;

        using AbstractRelicBatchSourcePtr = std::unique_ptr<AbstractRelicBatchSourceBase>;
        using AbstractRelicBatchSourceList = std::unordered_map<TypeHandle, AbstractRelicBatchSourcePtr>;
        AbstractRelicBatchSourceList abstractRelicBatchSources;

        // Will return an already created source if it exists
        template<class T, std::enable_if_t<!std::is_abstract_v<T>, int> = 0>
        RelicBatchSource<T>* CreateRelicBatchSource();
        // Will return an already created source if it exists
        template<class T, std::enable_if_t<std::is_abstract_v<T>, int> = 0>
        AbstractRelicBatchSource<T>* CreateRelicBatchSource();

        RelicBatchSourceBase* FindRelicBatchSource(const TypeHandle& typeHandle);
        AbstractRelicBatchSourceBase* FindAbstractRelicBatchSource(const TypeHandle& typeHandle);

        template<class T, std::enable_if_t<!std::is_abstract_v<T>, int> = 0>
        RelicBatchSource<T>* FindRelicBatchSource();
        template<class T, std::enable_if_t<std::is_abstract_v<T>, int> = 0>
        AbstractRelicBatchSource<T>* FindRelicBatchSource();
    private:
        using RelicFactoryPtr = std::unique_ptr<RelicFactoryBase>;
        using RelicFactoryList = std::unordered_map<TypeHandle, RelicFactoryPtr>;

        template<class RelicT>
        using RelicFactory = typename ProcessedRelicTraits<RelicT>::Factory;

        RelicFactoryList relicFactories;

        template<class RelicT>
        RelicFactory<RelicT>* CreateRelicFactory();
        template<class RelicT>
        RelicFactory<RelicT>* FindRelicFactory();
    private:
        using CuratorProviderPtr = std::unique_ptr<CuratorProviderBase>;
        using CuratorProviderMap = std::unordered_map<TypeHandle, CuratorProviderPtr>;
        CuratorProviderMap curatorProviders;

        using CuratorHandlePtr = std::unique_ptr<CuratorHandle>;
        using CuratorList = std::vector<CuratorHandlePtr>;
        CuratorList curators;

        using CuratorLayoutList = std::vector<CuratorLayout>;
        CuratorLayoutList curatorLayouts;

        void CreateAllCurators();
        template<class Curator>
        [[nodiscard]] bool HasCurator() const;

        void DoOnCurators(const std::function<void(Curator*)>& function);

        template<class Curator, class CuratorProvider, class... Args>
        void RegisterCuratorTypeCommon(Args && ... args);

        template<class Curator>
        static TypeHandle TypeHandleForCurator();
    private:
        using SignalBatchSourcePtr = std::unique_ptr<SignalBatchSourceBase>;
        using SignalBatchSourceList = std::unordered_map<std::type_index, SignalBatchSourcePtr>;

        SignalBatchSourceList signalBatchSources;

        template<class SignalT>
        SignalBatchSource<SignalT>* CreateSignalBatchSource();
        SignalBatchSourceBase* FindSignalBatchSource(const std::type_index& type);
        template<class SignalT>
        SignalBatchSource<SignalT>* FindSignalBatchSource();
        template<class SignalT>
        const SignalBatchSource<SignalT>* FindSignalBatchSource() const;
        template<class SignalT>
        void DestroySignalBatchSource();

        template<class SignalT>
        static std::type_index KeyForSignalBatchSource();

        template<class SignalT>
        [[nodiscard]] bool IsSignalRegistered() const;
    private:
        friend class TypeRegistration;
        template<class T>
        friend class RelicBatchSource;
        template<class T>
        friend class SignalBatchSource;
    private:
        INSCRIPTION_ACCESS;
    };

    template<
        class RelicT,
        class... Args,
        std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic && !std::is_abstract_v<RelicT>, int>
    > Ref<RelicT> Reliquary::CreateRelic(Args&& ... args)
    {
        if (!isInitialized)
            throw NotInitialized();

        auto created = CreateRelicCommon<RelicT>(std::forward<Args>(args)...);
        SignalCreation(created);
        return created;
    }

    template<class RelicT, std::enable_if_t<ProcessedRelicTraits<RelicT>::isStatic, int>>
    Ref<RelicT> Reliquary::StaticRelic()
    {
        if (!isInitialized)
            throw NotInitialized();

        auto batchSource = FindRelicBatchSource<RelicT>();
        if (!batchSource)
            throw NotRegistered();

        return batchSource->Find(staticRelicID);
    }

    template<class RelicT, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int>>
    void Reliquary::DestroyRelic(Ref<RelicT>& destroy)
    {
        if (!isInitialized)
            throw NotInitialized();

        auto batchSource = FindRelicBatchSource<RelicT>();
        if (!batchSource)
            throw NotRegistered();

        batchSource->Destroy(destroy);
        NotifyDestroyedAbstractRelicBatchSources(destroy);
    }

    template<class RelicT, std::enable_if_t<!std::is_abstract_v<RelicT>, int>>
    RelicBatch<RelicT> Reliquary::StartRelicBatch()
    {
        if (!isInitialized)
            throw NotInitialized();

        auto batchSource = FindRelicBatchSource<RelicT>();
        if (!batchSource)
            throw NotRegistered();

        return RelicBatch<RelicT>(*batchSource);
    }

    template<class RelicT, std::enable_if_t<std::is_abstract_v<RelicT>, int>>
    AbstractRelicBatch<RelicT> Reliquary::StartRelicBatch()
    {
        if (!isInitialized)
            throw NotInitialized();

        auto batchSource = FindRelicBatchSource<RelicT>();
        if (!batchSource)
            throw NotRegistered();

        return AbstractRelicBatch<RelicT>(*batchSource);
    }

    template<class CuratorT>
    CuratorT* Reliquary::FindCurator()
    {
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(CuratorT);

        if (!isInitialized)
            throw NotInitialized();

        for (auto& loop : curators)
        {
            auto casted = dynamic_cast<CuratorT*>(loop->Get());
            if (casted)
                return casted;
        }

        return nullptr;
    }

    template<class CuratorT>
    const CuratorT* Reliquary::FindCurator() const
    {
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(CuratorT);

        if (!isInitialized)
            throw NotInitialized();

        for (auto& loop : curators)
        {
            auto casted = dynamic_cast<const CuratorT*>(loop->Get());
            if (casted)
                return casted;
        }

        return nullptr;
    }

    template<class RelicT, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int>>
    void Reliquary::RegisterRelicType()
    {
        RegisterRelicTypeCommon<RelicT>();

        RegisterSignalType<RelicCreated<RelicT>>();
        RegisterSignalType<BeforeRelicDestroyed<RelicT>>();
    }

    template<class RelicT, class... Args, std::enable_if_t<ProcessedRelicTraits<RelicT>::isStatic, int>>
    void Reliquary::RegisterRelicType(Args&& ... args)
    {
        RegisterRelicTypeCommon<RelicT>();

        CreateStaticRelicInitializer<RelicT>(std::forward<Args>(args)...);
    }

    template<class T, class... Args>
    void Reliquary::RegisterCuratorType(Args&& ... args)
    {
        RegisterCuratorTypeCommon<T, CuratorProvider<T, Args...>>(std::make_tuple(std::forward<Args>(args)...));
    }

    template<class T, class CuratorT, typename std::enable_if<std::is_base_of_v<CuratorT, Curator>, int>::type>
    void Reliquary::RegisterCuratorType(CuratorT* use)
    {
        if (!dynamic_cast<T>(use))
            throw IncorrectRegisteredCuratorType();

        RegisterCuratorTypeCommon<T, ExternalCuratorProvider>(use, CuratorTraits<CuratorT>::typeHandle);
    }

    template<class SignalT>
    void Reliquary::RaiseSignal(const SignalT& signal)
    {
        if (!isInitialized)
            throw NotInitialized();

        auto signalBatchSource = FindSignalBatchSource<SignalT>();
        if (!signalBatchSource)
            throw NotRegistered();

        signalBatchSource->Raise(signal);
    }

    template<class SignalT>
    SignalBatch<SignalT> Reliquary::StartSignalBatch()
    {
        auto signalBatchSource = FindSignalBatchSource<SignalT>();
        if (!signalBatchSource)
            throw NotRegistered();

        return SignalBatch<SignalT>(*signalBatchSource);
    }

    template<class SignalT>
    void Reliquary::RegisterSignalType()
    {
        if (isInitialized)
            throw CannotRegister();

        if (IsSignalRegistered<SignalT>())
            throw AlreadyRegistered();

        CreateSignalBatchSource<SignalT>();
    }

    template<class RelicT, class... Args>
    Ref<RelicT> Reliquary::CreateRelicCommon(Args&& ... args)
    {
        auto factory = FindRelicFactory<RelicT>();
        auto batchSource = FindRelicBatchSource<RelicT>();
        if (!factory || !batchSource)
            throw NotRegistered();

        auto created = factory->CreateRelic(std::forward<Args>(args)...);
        if (!created)
            return {};

        auto added = batchSource->Add(std::move(*created));
        NotifyAddedAbstractRelicBatchSources(added);
        return added;
    }

    template<class RelicT>
    void Reliquary::NotifyAddedAbstractRelicBatchSources(Ref<RelicT>& relic)
    {
        auto bases = relicTypeGraph.AllBasesFor(TypeHandleForRelic<RelicT>());
        for (auto& loop : bases)
        {
            auto found = FindAbstractRelicBatchSource(loop.typeHandle);
            if (!found)
                continue;
            found->NotifyAdded(&*relic);
        }
    }

    template<class RelicT>
    void Reliquary::NotifyDestroyedAbstractRelicBatchSources(Ref<RelicT>& relic)
    {
        auto bases = relicTypeGraph.AllBasesFor(TypeHandleForRelic<RelicT>());
        for (auto& loop : bases)
        {
            auto found = FindAbstractRelicBatchSource(loop.typeHandle);
            if (!found)
                continue;
            found->NotifyDestroyed(&*relic);
        }
    }

    template<class RelicT>
    void Reliquary::SignalCreation(const Ref<RelicT>& relic)
    {
        RelicCreated<RelicT> signal{ relic };
        RaiseSignal(signal);
    }

    template<class RelicT>
    void Reliquary::SignalDestruction(const Ref<RelicT>& relic)
    {
        BeforeRelicDestroyed<RelicT> signal{ relic };
        RaiseSignal(signal);
    }

    template<class RelicT, class... Args, std::enable_if_t<ProcessedRelicTraits<RelicT>::isStatic, int>>
    void Reliquary::CreateStaticRelicInitializer(Args&& ... args)
    {
        staticRelicInitializerList.push_back([](Reliquary& reliquary)
        {
            reliquary.CreateRelicCommon<RelicT>(std::forward<Args>(args)...);
        });
    }

    template<class RelicT>
    void Reliquary::RegisterRelicTypeCommon()
    {
        if (isInitialized)
            throw CannotRegister();

        if (relicTypeGraph.HasTypeHandle(TypeHandleForRelic<RelicT>()))
            throw AlreadyRegistered();

        relicTypeGraph.AddDescription(TypeDescriptionForRelic<RelicT>());
        CreateRelicBatchSource<RelicT>();
        CreateRelicFactory<RelicT>();
    }

    template<class RelicT>
    [[nodiscard]] bool Reliquary::IsRelicRegistered() const
    {
        return relicTypeGraph.HasTypeHandle(TypeHandleForRelic<RelicT>());
    }

    template<class RelicT>
    TypeHandle Reliquary::TypeHandleForRelic()
    {
        return RelicTraits<RelicT>::typeHandle;
    }

    template<class RelicT>
    RelicTypeDescription Reliquary::TypeDescriptionForRelic()
    {
        return ProcessedRelicTraits<RelicT>::TypeDescription();
    }

    template<class RelicT>
    auto Reliquary::CreateRelicFactory() -> RelicFactory<RelicT>*
    {
        using FactoryT = RelicFactory<RelicT>;

        auto found = FindRelicFactory<RelicT>();
        if (found)
            return found;

        auto relicTypeName = TypeHandleForRelic<RelicT>();

        auto made = new FactoryT(*this);
        relicFactories.emplace(relicTypeName, RelicFactoryPtr(made));
        return made;
    }

    template<class RelicT>
    auto Reliquary::FindRelicFactory() -> RelicFactory<RelicT>*
    {
        using FactoryT = RelicFactory<RelicT>;

        auto relicTypeHandle = TypeHandleForRelic<RelicT>();

        auto found = relicFactories.find(relicTypeHandle);
        if (found == relicFactories.end())
            return nullptr;

        return static_cast<FactoryT*>(found->second.get());
    }

    template<class Curator>
    bool Reliquary::HasCurator() const
    {
        if (curatorProviders.find(TypeHandleForCurator<Curator>()) != curatorProviders.end())
            return true;

        return FindCurator<Curator>() != nullptr;
    }

    template<class Curator, class CuratorProvider, class... Args>
    void Reliquary::RegisterCuratorTypeCommon(Args && ... args)
    {
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(Curator);

        if (isInitialized)
            throw CannotRegister();

        auto typeHandle = TypeHandleForCurator<Curator>();

        if (curatorProviders.find(typeHandle) != curatorProviders.end())
            throw AlreadyRegistered();

        auto provider = new CuratorProvider(std::forward<Args>(args)...);
        curatorProviders.emplace(typeHandle, CuratorProviderPtr(provider));
    }

    template<class Curator>
    TypeHandle Reliquary::TypeHandleForCurator()
    {
        return CuratorTraits<Curator>::typeHandle;
    }

    template<class T, std::enable_if_t<!std::is_abstract_v<T>, int>>
    RelicBatchSource<T>* Reliquary::CreateRelicBatchSource()
    {
        auto found = FindRelicBatchSource<T>();
        if (found)
            return found;

        auto typeHandle = TypeHandleForRelic<T>();

        auto created = std::make_unique<RelicBatchSource<T>>();
        auto emplaced = relicBatchSources.emplace(typeHandle, std::move(created)).first->second.get();
        return static_cast<RelicBatchSource<T>*>(emplaced);
    }

    template<class T, std::enable_if_t<std::is_abstract_v<T>, int>>
    AbstractRelicBatchSource<T>* Reliquary::CreateRelicBatchSource()
    {
        auto found = FindRelicBatchSource<T>();
        if (found)
            return found;

        auto typeHandle = TypeHandleForRelic<T>();

        auto created = std::make_unique<AbstractRelicBatchSource<T>>();
        auto emplaced = abstractRelicBatchSources.emplace(typeHandle, std::move(created)).first->second.get();
        return static_cast<AbstractRelicBatchSource<T>*>(emplaced);
    }

    template<class T, std::enable_if_t<!std::is_abstract_v<T>, int>>
    RelicBatchSource<T>* Reliquary::FindRelicBatchSource()
    {
        auto found = relicBatchSources.find(TypeHandleForRelic<T>());
        if (found == relicBatchSources.end())
            return nullptr;

        return static_cast<RelicBatchSource<T>*>(found->second.get());
    }

    template<class T, std::enable_if_t<std::is_abstract_v<T>, int>>
    AbstractRelicBatchSource<T>* Reliquary::FindRelicBatchSource()
    {
        auto found = abstractRelicBatchSources.find(TypeHandleForRelic<T>());
        if (found == abstractRelicBatchSources.end())
            return nullptr;

        return static_cast<AbstractRelicBatchSource<T>*>(found->second.get());
    }

    template<class SignalT>
    SignalBatchSource<SignalT>* Reliquary::CreateSignalBatchSource()
    {
        auto found = FindSignalBatchSource<SignalT>();
        if (found)
            return found;

        auto created = new SignalBatchSource<SignalT>(*this);
        signalBatchSources.emplace(KeyForSignalBatchSource<SignalT>(), SignalBatchSourcePtr(created));
        return created;
    }

    template<class SignalT>
    SignalBatchSource<SignalT>* Reliquary::FindSignalBatchSource()
    {
        auto found = signalBatchSources.find(KeyForSignalBatchSource<SignalT>());
        if (found == signalBatchSources.end())
            return nullptr;

        return static_cast<SignalBatchSource<SignalT>*>(found->second.get());
    }

    template<class SignalT>
    const SignalBatchSource<SignalT>* Reliquary::FindSignalBatchSource() const
    {
        auto found = signalBatchSources.find(KeyForSignalBatchSource<SignalT>());
        if (found == signalBatchSources.end())
            return nullptr;

        return static_cast<SignalBatchSource<SignalT>*>(found->second.get());
    }

    template<class SignalT>
    void Reliquary::DestroySignalBatchSource()
    {
        signalBatchSources.erase(KeyForSignalBatchSource<SignalT>());
    }

    template<class SignalT>
    std::type_index Reliquary::KeyForSignalBatchSource()
    {
        return typeid(SignalT);
    }

    template<class SignalT>
    bool Reliquary::IsSignalRegistered() const
    {
        return FindSignalBatchSource<SignalT>() != nullptr;
    }
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::Reliquary, BinaryArchive> final :
        public CompositeScribe<::Arca::Reliquary, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    private:
        static void Save(ObjectT& object, ArchiveT& archive);
        static void Load(ObjectT& object, ArchiveT& archive);

        template<class Descriptions>
        static bool ShouldLoadRelic(const TypeHandle& typeHandle, const Descriptions& descriptions, ArchiveT& archive);
    };

    template<class Descriptions>
    bool Scribe<::Arca::Reliquary, BinaryArchive>::ShouldLoadRelic(const TypeHandle& typeHandle, const Descriptions& descriptions, ArchiveT& archive)
    {
        const auto anyRepresentedIsLoaded = [typeHandle, &archive](const ::Arca::RelicTypeDescription& relicDescription)
        {
            auto representedTypeHandles = relicDescription.AllSerializationRepresentedTypeHandles(archive);
            const auto anyRepresentedIsLoaded = [typeHandle](const TypeHandle& representedTypeHandle)
            {
                return representedTypeHandle == typeHandle;
            };
            return std::any_of(representedTypeHandles.begin(), representedTypeHandles.end(), anyRepresentedIsLoaded);
        };

        return std::any_of(descriptions.begin(), descriptions.end(), anyRepresentedIsLoaded);
    }
}

#include "SignalBatchSourceDefinition.h"