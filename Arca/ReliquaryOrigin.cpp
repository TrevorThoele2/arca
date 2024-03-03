#include "ReliquaryOrigin.h"

#include <unordered_set>

namespace Arca
{
    ReliquaryOrigin::ReliquaryOrigin(const ReliquaryOrigin& arg) :
        relicList(arg.relicList),
        staticRelicList(arg.staticRelicList),
        namedRelicStructureList(arg.namedRelicStructureList),
        shardList(arg.shardList),
        curatorPipeline(arg.curatorPipeline),
        curatorSerializationTypeHandlesFactoryList(arg.curatorSerializationTypeHandlesFactoryList),
        signalList(arg.signalList)
    {
        for (auto& provider : arg.curatorProviders)
            curatorProviders.emplace(provider.first, provider.second->Clone());
    }

    ReliquaryOrigin& ReliquaryOrigin::operator=(const ReliquaryOrigin& arg)
    {
        relicList = arg.relicList;
        staticRelicList = arg.staticRelicList;
        namedRelicStructureList = arg.namedRelicStructureList;
        shardList = arg.shardList;
        for (auto& provider : arg.curatorProviders)
            curatorProviders.emplace(provider.first, provider.second->Clone());
        curatorPipeline = arg.curatorPipeline;
        curatorSerializationTypeHandlesFactoryList = arg.curatorSerializationTypeHandlesFactoryList;
        signalList = arg.signalList;

        return *this;
    }

    Reliquary ReliquaryOrigin::Actualize() const
    {
        Reliquary reliquary;

        for (auto& initializer : signalList)
            initializer.factory(reliquary);

        for (auto& initializer : shardList)
            initializer.factory(reliquary);

        for (auto& initializer : relicList)
            initializer.factory(reliquary);

        reliquary.namedRelicStructureList = namedRelicStructureList;

        PushAllCuratorsTo(reliquary);

        PushCuratorPipeline(reliquary);

        for (auto& initializer : staticRelicList)
            initializer.factory(reliquary);

        {
            for (auto& loop : reliquary.curators)
                loop.second->Get()->Initialize();

            for (auto& loop : typedRelicInitializerList)
                loop(reliquary);
        }

        return reliquary;
    }

    ReliquaryOrigin& ReliquaryOrigin::RelicStructure(const std::string& name, const Arca::RelicStructure& structure)
    {
        for (auto& loop : namedRelicStructureList)
            if (loop.name == name)
                throw AlreadyRegistered();

        namedRelicStructureList.emplace_back(name, structure);
        return *this;
    }

    ReliquaryOrigin& ReliquaryOrigin::CuratorPipeline(const Arca::CuratorPipeline& pipeline)
    {
        curatorPipeline = pipeline;

        return *this;
    }

    void ReliquaryOrigin::PushAllCuratorsTo(Reliquary& reliquary) const
    {
        std::vector<CuratorProviderBase::Provided> provided;

        for (auto& loop : curatorProviders)
            provided.push_back(loop.second->Provide(reliquary));

        for (auto& loop : provided)
        {
            Reliquary::CuratorHandlePtr handle;

            if (loop.curator.index() == 0)
                handle = std::make_unique<OwnedCuratorHandle>(std::move(std::get<0>(loop.curator)), loop.typeHandle);
            else
                handle = std::make_unique<UnownedCuratorHandle>(std::get<1>(loop.curator), loop.typeHandle);

            reliquary.curators.emplace(handle->typeHandle, std::move(handle));
        }

        for (auto& loop : curatorSerializationTypeHandlesFactoryList)
            loop(reliquary);
    }

    void ReliquaryOrigin::PushCuratorPipeline(Reliquary& reliquary) const
    {
        Reliquary::CuratorPipeline createdPipeline;

        std::unordered_set<Arca::Curator*> seenCurators;

        for(auto& stage : curatorPipeline)
        {
            Reliquary::CuratorStage createdStage;

            auto typeHandles = stage.TypeHandleList();
            for(auto& typeHandle : typeHandles)
            {
                auto found = reliquary.FindCurator(typeHandle);
                if (found == nullptr)
                    throw InvalidCuratorPipeline("Curator (" + typeHandle + ") was not found.");

                const auto wasAlreadySeen = !seenCurators.emplace(found).second;
                if (wasAlreadySeen)
                    throw InvalidCuratorPipeline("Curator (" + typeHandle + ") was already in the pipeline.");

                createdStage.push_back(found);
            }

            if (!createdStage.empty())
                createdPipeline.push_back(createdStage);
        }

        reliquary.curatorPipeline = createdPipeline;
    }
}