#include "Reliquary.h"

#include <unordered_set>

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>
#include <Inscription/OutputJumpTable.h>
#include <Inscription/InputJumpTable.h>

namespace Arca
{
    Reliquary::Reliquary(Reliquary&& arg) noexcept :
        relics(std::move(arg.relics)), relicToExtensionMap(std::move(arg.relicToExtensionMap)),
        staticRelicInitializerList(std::move(arg.staticRelicInitializerList)),
        relicTypeGraph(std::move(arg.relicTypeGraph)), relicFactories(std::move(arg.relicFactories)),
        curators(std::move(arg.curators)), relicBatchSources(std::move(arg.relicBatchSources)),
        signalBatchSources(std::move(arg.signalBatchSources))
    {
        for (auto& loop : relics)
            loop->owner = this;

        for (auto& loop : relicFactories)
            loop.second->owner = this;

        for (auto& loop : curators)
            loop->Get()->owner = this;

        for (auto& loop : relicBatchSources)
            loop.second->owner = this;

        for (auto& loop : signalBatchSources)
            loop.second->owner = this;
    }

    Reliquary& Reliquary::operator=(Reliquary&& arg) noexcept
    {
        relicTypeGraph = std::move(arg.relicTypeGraph);
        relics = std::move(arg.relics);
        relicToExtensionMap = std::move(arg.relicToExtensionMap);
        staticRelicInitializerList = std::move(arg.staticRelicInitializerList);
        relicFactories = std::move(arg.relicFactories);
        curators = std::move(arg.curators);
        relicBatchSources = std::move(arg.relicBatchSources);
        return *this;
    }

    void Reliquary::Initialize(const std::function<void(Reliquary&)>& afterCuratorsCreated)
    {
        if (isInitialized)
            return;

        CreateAllStaticRelics();
        staticRelicInitializerList.clear();

        CreateAllCurators();

        afterCuratorsCreated(*this);

        for (auto& loop : curators)
            loop->Get()->Initialize();

        isInitialized = true;
    }

    void Reliquary::Work()
    {
        if (!isInitialized)
            throw NotInitialized();

        for (auto& loop : curators)
            loop->Get()->Work();
    }

    Reliquary::SizeT Reliquary::RelicCount() const
    {
        return relics.Size();
    }

    RelicTypeDescriptionGroup Reliquary::DescriptionGroupForRelic(const TypeHandle& typeHandle) const
    {
        return relicTypeGraph.GroupFor(typeHandle);
    }

    RelicTypeGraph Reliquary::RelicTypeGraph() const
    {
        return relicTypeGraph;
    }

    std::vector<CuratorTypeDescription> Reliquary::CuratorTypeDescriptions() const
    {
        std::vector<CuratorTypeDescription> returnValue;
        for (auto& loop : curators)
            returnValue.push_back(loop->description);
        return returnValue;
    }

    void Reliquary::RegisterCuratorLayout(const CuratorLayout& layout)
    {
        if (isInitialized)
            throw CannotRegister();

        curatorLayouts.push_back(layout);
    }

    AnyExtendedRelic* Reliquary::AddRelic(RelicPtr&& relic)
    {
        auto added = &**relics.Add(relic->id, std::move(relic));
        relicToExtensionMap.emplace(added->RelicMemory(), added->Extension());
        return added;
    }

    void Reliquary::CreateAllStaticRelics()
    {
        for (auto& loop : staticRelicInitializerList)
            loop(*this);
    }

    void Reliquary::CreateAllCurators()
    {
        CuratorProviderBase::CuratorMap createdCurators;
        CuratorProviderBase::CuratorProviderMap storedProviders;

        std::vector<CuratorProviderBase::Provided> provided;

        for (auto& loop : curatorProviders)
            provided.push_back(loop.second->Provide(createdCurators, storedProviders, *this));

        for (auto& loop : provided)
        {
            CuratorHandlePtr handle;

            if (loop.curator.index() == 0)
                handle = std::make_unique<OwnedCuratorHandle>(std::move(std::get<0>(loop.curator)), loop.description);
            else
                handle = std::make_unique<UnownedCuratorHandle>(std::get<1>(loop.curator), loop.description);

            curators.push_back(std::move(handle));
        }

        curatorProviders.clear();
    }

    void Reliquary::DoOnCurators(const std::function<void(Curator*)>& function)
    {
        using RelicMap = std::unordered_set<std::type_index>;
        RelicMap used;

        const auto runFunction = [&used, &function](const std::type_index& type, Curator* curator)
        {
            function(curator);
            used.emplace(type);
        };

        for (auto& currentLayout : curatorLayouts)
        {
            for (auto& currentCuratorType : currentLayout)
            {
                if (used.find(currentCuratorType) != used.end())
                    continue;

                auto handle = std::find_if(curators.begin(), curators.end(), [&currentCuratorType](CuratorHandlePtr& handle)
                {
                    return handle->Type() == currentCuratorType;
                });
                assert(handle != curators.end());
                runFunction(currentCuratorType, (*handle)->Get());
            }
        }

        for (auto& loop : curators)
        {
            if (used.find(loop->Type()) != used.end())
                continue;

            runFunction(loop->Type(), loop->Get());
        }
    }

    RelicBatchSourceBase* Reliquary::FindRelicBatchSource(const TypeHandle& typeHandle)
    {
        const auto found = relicBatchSources.find(typeHandle);
        if (found == relicBatchSources.end())
            return nullptr;

        return found->second.get();
    }

    void Reliquary::NotifyRelicBatchSourcesCreation(AnyExtendedRelic& relic)
    {
        for (auto& loop : relicBatchSources)
            loop.second->NotifyCreated(relic);
    }

    void Reliquary::NotifyRelicBatchSourcesDestruction(AnyExtendedRelic& relic)
    {
        for (auto& loop : relicBatchSources)
            loop.second->NotifyDestroyed(relic);
    }

    SignalBatchSourceBase* Reliquary::FindSignalBatchSource(const std::type_index& type)
    {
        const auto found = signalBatchSources.find(type);
        if (found == signalBatchSources.end())
            return nullptr;

        return found->second.get();
    }
}

namespace Inscription
{
    void Scribe<::Arca::Reliquary, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        if (archive.IsOutput())
            Save(object, archive);
        else
            Load(object, archive);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Save(ObjectT& object, ArchiveT& archive)
    {
        // Relics
        {
            OutputJumpTable<::Arca::TypeHandle, ::Arca::ExtendedRelicSerializer> jumpTable;
            for(auto& loop : )
            std::unordered_map<::Arca::TypeHandle, RelicList> map;
            for (auto& loop : object.relics)
                map[loop->TypeDescription().typeHandle].push_back(RelicPtr(loop.get()));
            for (auto& relicType : map)
                jumpTable.Add(relicType.first, relicType.second);
            archive(jumpTable);
            for (auto& relicList : map)
                for (auto& loop : relicList.second)
                    loop.release();
        }

        // Curators
        /*
        {
            OutputJumpTable<::Arca::TypeHandle, ::Arca::Curator&> jumpTable;
            std::vector<::Arca::Curator*> curators;
            for (auto& loop : object.curators)
            {
                if (!loop->IsOwning())
                    continue;

                curators.push_back(loop->Get());
                jumpTable.Add(loop->description.typeHandle, *curators.back());
            }
            archive(jumpTable);
        }
        */
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Load(ObjectT& object, ArchiveT& archive)
    {
        if (object.isInitialized)
            throw ::Arca::AttemptedLoadWhileInitialized();

        // Relics
        {
            using RelicPtr = std::unique_ptr<::Arca::AnyExtendedRelic>;
            using RelicList = std::vector<RelicPtr>;
            InputJumpTable<::Arca::TypeHandle, RelicList> jumpTable;
            archive(jumpTable);

            const auto allDescriptions = object.RelicTypeGraph().AllDescriptions();
            for(auto& loadedTypeHandle : jumpTable.AllIDs())
            {
                const auto shouldLoad = ShouldLoadRelic(loadedTypeHandle, allDescriptions, archive);

                if (!shouldLoad)
                    continue;

                auto relicList = jumpTable.ConstructObject(loadedTypeHandle, archive);
                for (auto& relic : *relicList)
                    object.AddRelic(std::move(relic));
            }
        }

        object.staticRelicInitializerList.clear();
        object.CreateAllCurators();

        // Curators
        {
            InputJumpTable<::Arca::TypeHandle, ::Arca::Curator&> jumpTable;
            archive(jumpTable);

            for (auto& loadedTypeHandle : jumpTable.AllIDs())
            {
                const auto anyRepresentedIsLoaded = [loadedTypeHandle, &archive](const std::unique_ptr<::Arca::CuratorHandle>& curator)
                {
                    auto representedTypeHandles = curator->description.AllSerializationRepresentedTypeHandles(archive);
                    const auto anyRepresentedIsLoaded = [loadedTypeHandle](const TypeHandle& representedTypeHandle)
                    {
                        return representedTypeHandle == loadedTypeHandle;
                    };
                    return std::any_of(representedTypeHandles.begin(), representedTypeHandles.end(), anyRepresentedIsLoaded);
                };

                auto foundCurator = std::find_if(object.curators.begin(), object.curators.end(), anyRepresentedIsLoaded);
                if (foundCurator == object.curators.end())
                    continue;

                jumpTable.FillObject(*(*foundCurator)->Get(), loadedTypeHandle, archive);
            }
        }

        object.isInitialized = true;
    }
}