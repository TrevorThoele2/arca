#pragma once

#include <unordered_map>
#include <optional>

#include "ReliquaryException.h"

#include "ExtendedRelic.h"
#include "ExtendedRelicSerializer.h"
#include "BasicRelicFactory.h"
#include "RelicBatch.h"
#include "RelicBatchSource.h"
#include "ProcessedRelicTraits.h"

#include "SignalBatch.h"
#include "SignalBatchSource.h"
#include "RelicCreated.h"
#include "BeforeRelicDestroyed.h"

#include "RelicTraits.h"
#include "RelicTypeGraph.h"

#include "Curator.h"
#include "CuratorProvider.h"
#include "CuratorLayout.h"
#include "CuratorHandle.h"
#include "ProcessedCuratorTraits.h"

#include "IDManager.h"

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
        template<class RelicT, class... Args, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int> = 0>
        RelicT& CreateRelic(Args&& ... args);
        template<class RelicT, class... Args, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int> = 0>
        RelicT* CreateRelicWithID(RelicID id, Args&& ... args);

        template<class RelicT>
        [[nodiscard]] RelicT* FindRelic(RelicID id);
        template<class RelicT, std::enable_if_t<ProcessedRelicTraits<RelicT>::isStatic, int> = 0>
        [[nodiscard]] RelicT* StaticRelic();

        template<class RelicT, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int> = 0>
        void DestroyRelic(RelicT& relic);

        [[nodiscard]] SizeT RelicCount() const;

        template<class RelicT>
        [[nodiscard]] std::optional<RelicExtension> ExtensionFor(RelicT& relic) const;
        template<class RelicT>
        [[nodiscard]] std::optional<RelicTypeDescription> TypeDescriptionFor(RelicT& relic) const;
        template<class RelicT>
        [[nodiscard]] std::optional<RelicID> IDFor(RelicT& relic) const;

        [[nodiscard]] RelicTypeDescriptionGroup DescriptionGroupForRelic(const TypeHandle& typeHandle) const;
        [[nodiscard]] RelicTypeGraph RelicTypeGraph() const;

        template<class RelicT>
        RelicBatch<RelicT> StartRelicBatch();

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
        using RelicPtr = std::unique_ptr<AnyExtendedRelic>;
        using RelicList = IDManager<RelicID, RelicPtr>;
        RelicList relics;

        using RelicToExtensionMap = std::unordered_map<void*, RelicExtension>;
        RelicToExtensionMap relicToExtensionMap;

        using StaticRelicInitializer = void(*)(Reliquary&);
        using StaticRelicInitializerList = std::vector<StaticRelicInitializer>;
        StaticRelicInitializerList staticRelicInitializerList;

        using StaticRelicMap = std::unordered_map<std::type_index, AnyExtendedRelic*>;
        StaticRelicMap staticRelicMap;

        Arca::RelicTypeGraph relicTypeGraph;

        template<class RelicT, class... Args>
        ExtendedRelic<RelicT>* CreateRelicCommon(const std::optional<RelicID>& id, Args&& ... args);
        AnyExtendedRelic* AddRelic(RelicPtr&& relic);

        template<class RelicT>
        void NotifyCreation(RelicT& relic);
        template<class RelicT>
        void NotifyDestruction(RelicT& relic);

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
        using ExtendedRelicSerializerMap = std::unordered_map<TypeHandle, ExtendedRelicSerializer>;
        ExtendedRelicSerializerMap extendedRelicSerializerMap;

        template<class RelicT>
        void CreateExtendedRelicSerializer();
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
        using RelicBatchSourcePtr = std::unique_ptr<RelicBatchSourceBase>;
        using RelicBatchSourceList = std::unordered_map<TypeHandle, RelicBatchSourcePtr>;

        RelicBatchSourceList relicBatchSources;

        // Will return an already created source if it exists
        template<class T>
        RelicBatchSource<T>* CreateRelicBatchSource();
        RelicBatchSourceBase* FindRelicBatchSource(const TypeHandle& typeHandle);
        template<class T>
        RelicBatchSource<T>* FindRelicBatchSource();
        template<class T>
        void DestroyRelicBatchSource();
        void NotifyRelicBatchSourcesCreation(AnyExtendedRelic& relic);
        void NotifyRelicBatchSourcesDestruction(AnyExtendedRelic& relic);
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

    template<class RelicT, class... Args, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int>>
    RelicT& Reliquary::CreateRelic(Args&& ... args)
    {
        if (!isInitialized)
            throw NotInitialized();

        auto created = CreateRelicCommon<RelicT>(std::optional<RelicID>(), std::forward<Args>(args)...);
        NotifyRelicBatchSourcesCreation(*created);
        NotifyCreation(created->relic);
        return created->relic;
    }

    template<class RelicT, class... Args, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int>>
    RelicT* Reliquary::CreateRelicWithID(RelicID id, Args&& ... args)
    {
        if (!isInitialized)
            throw NotInitialized();

        auto created = CreateRelicCommon<RelicT>(id, std::forward<Args>(args)...);
        NotifyRelicBatchSourcesCreation(*created);
        NotifyCreation(created->relic);
        return &created->relic;
    }

    template<class RelicT>
    RelicT* Reliquary::FindRelic(RelicID id)
    {
        for (auto& loop : relics)
        {
            if (loop->id != id)
                continue;

            auto casted = static_cast<ExtendedRelic<RelicT>*>(loop.get());
            return &casted->relic;
        }

        return nullptr;
    }

    template<class RelicT, std::enable_if_t<ProcessedRelicTraits<RelicT>::isStatic, int>>
    RelicT* Reliquary::StaticRelic()
    {
        if (!isInitialized)
            throw NotInitialized();

        auto found = staticRelicMap.find(typeid(RelicT));
        if (found == staticRelicMap.end())
            return nullptr;

        auto casted = static_cast<ExtendedRelic<RelicT>*>(found->second);
        return &casted->relic;
    }

    template<class RelicT, std::enable_if_t<!ProcessedRelicTraits<RelicT>::isStatic, int>>
    void Reliquary::DestroyRelic(RelicT& relic)
    {
        for (auto loop = relics.begin(); loop != relics.end(); ++loop)
        {
            if ((*loop)->RelicMemory() != &relic)
                continue;

            auto casted = static_cast<ExtendedRelic<RelicT>*>(loop->get());
            NotifyRelicBatchSourcesDestruction(**loop);
            NotifyDestruction(casted->relic);
            relicToExtensionMap.erase(casted->RelicMemory());
            relics.Remove(casted->id);
            return;
        }
    }

    template<class RelicT>
    std::optional<RelicExtension> Reliquary::ExtensionFor(RelicT& relic) const
    {
        auto found = relicToExtensionMap.find(&relic);
        if (found == relicToExtensionMap.end())
            return {};

        return found->second;
    }

    template<class RelicT>
    std::optional<RelicTypeDescription> Reliquary::TypeDescriptionFor(RelicT& relic) const
    {
        auto extension = ExtensionFor(relic);
        if (!extension)
            return {};

        return extension->typeDescription;
    }

    template<class RelicT>
    std::optional<RelicID> Reliquary::IDFor(RelicT& relic) const
    {
        auto extension = ExtensionFor(relic);
        if (!extension)
            return {};

        return extension->id;
    }

    template<class T>
    RelicBatch<T> Reliquary::StartRelicBatch()
    {
        if (!IsRelicRegistered<T>())
            throw NotRegistered();

        auto source = CreateRelicBatchSource<T>();
        return RelicBatch<T>(*source);
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
    ExtendedRelic<RelicT>* Reliquary::CreateRelicCommon(const std::optional<RelicID>& id, Args&& ... args)
    {
        auto factory = FindRelicFactory<RelicT>();
        if (!factory)
            throw NotRegistered();

        auto useId = (id) ?
            *id :
            relics.NextAutomaticID();
        auto created = factory->CreateRelic(std::forward<Args>(args)...);
        if (!created)
            return nullptr;

        auto added = AddRelic(std::make_unique<ExtendedRelic<RelicT>>(*created, useId, *this));
        return static_cast<ExtendedRelic<RelicT>*>(added);
    }

    template<class RelicT>
    void Reliquary::NotifyCreation(RelicT& relic)
    {
        RelicCreated<RelicT> signal{ relic };
        RaiseSignal(signal);
    }

    template<class RelicT>
    void Reliquary::NotifyDestruction(RelicT& relic)
    {
        BeforeRelicDestroyed<RelicT> signal{ relic };
        RaiseSignal(signal);
    }

    template<class RelicT, class... Args, std::enable_if_t<ProcessedRelicTraits<RelicT>::isStatic, int>>
    void Reliquary::CreateStaticRelicInitializer(Args&& ... args)
    {
        staticRelicInitializerList.push_back([](Reliquary& reliquary)
        {
            auto created = reliquary.CreateRelicCommon<RelicT>(std::optional<RelicID>(), std::forward<Args>(args)...);
            reliquary.staticRelicMap.emplace(typeid(RelicT), created);
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
        CreateRelicFactory<RelicT>();
        CreateExtendedRelicSerializer<RelicT>();
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
    void Reliquary::CreateExtendedRelicSerializer()
    {
        extendedRelicSerializerMap.emplace
        (
            RelicTraits<RelicT>::typeHandle,
            ExtendedRelicSerializer(::Chroma::Type<RelicT>{})
        );
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

    template<class T>
    RelicBatchSource<T>* Reliquary::CreateRelicBatchSource()
    {
        auto found = FindRelicBatchSource<T>();
        if (found)
            return found;

        auto typeHandle = TypeHandleForRelic<T>();

        auto created = new RelicBatchSource<T>(*this);
        relicBatchSources.emplace(typeHandle, RelicBatchSourcePtr(created));
        return created;
    }

    template<class T>
    RelicBatchSource<T>* Reliquary::FindRelicBatchSource()
    {
        auto found = relicBatchSources.find(TypeHandleForRelic<T>());
        if (found == relicBatchSources.end())
            return nullptr;

        return static_cast<RelicBatchSource<T>*>(found->second.get());
    }

    template<class T>
    void Reliquary::DestroyRelicBatchSource()
    {
        relicBatchSources.erase(TypeHandleForRelic<T>());
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

#include "RelicBatchSourceDefinition.h"
#include "SignalBatchSourceDefinition.h"