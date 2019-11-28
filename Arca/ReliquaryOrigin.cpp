#include "ReliquaryOrigin.h"

#include <unordered_set>

#include "RelicParented.h"
#include "PipelineException.h"

namespace Arca
{
    ReliquaryOrigin::ReliquaryOrigin()
    {
        Signal<CreatedRelic>();
        Signal<DestroyingRelic>();
        Signal<RelicParented>();
    }

    ReliquaryOrigin::ReliquaryOrigin(const ReliquaryOrigin& arg) :
        relicList(arg.relicList),
        staticRelicList(arg.staticRelicList),
        namedRelicStructureList(arg.namedRelicStructureList),
        shardList(arg.shardList),
        curatorInitializationPipeline(arg.curatorInitializationPipeline),
        curatorWorkPipeline(arg.curatorWorkPipeline),
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
        curatorInitializationPipeline = arg.curatorInitializationPipeline;
        curatorWorkPipeline = arg.curatorWorkPipeline;
        curatorSerializationTypeHandlesFactoryList = arg.curatorSerializationTypeHandlesFactoryList;
        signalList = arg.signalList;

        return *this;
    }

    std::unique_ptr<Reliquary> ReliquaryOrigin::Actualize() const
    {
        auto reliquary = std::make_unique<Reliquary>();

        for (auto& initializer : signalList)
            initializer.factory(*reliquary);

        for (auto& initializer : shardList)
            initializer.factory(*reliquary);

        for (auto& initializer : relicList)
            initializer.factory(*reliquary);

        reliquary->namedRelicStructureList = namedRelicStructureList;

        const auto allCurators = PushAllCuratorsTo(*reliquary);

        auto transformedCuratorInitializationPipeline =
            TransformCuratorPipeline(
                *reliquary,
                curatorInitializationPipeline,
                allCurators);
        reliquary->curatorWorkPipeline =
            TransformCuratorPipeline(
                *reliquary,
                curatorWorkPipeline,
                allCurators);

        for (auto& initializer : staticRelicList)
            initializer.factory(*reliquary);

        {
            for (auto& stage : transformedCuratorInitializationPipeline)
                for (auto& curator : stage)
                    curator->Initialize(*reliquary);

            for (auto& loop : typedRelicInitializerList)
                loop(*reliquary);
        }

        return reliquary;
    }

    ReliquaryOrigin& ReliquaryOrigin::RelicStructure(const std::string& name, const Arca::RelicStructure& structure)
    {
        for (auto& loop : namedRelicStructureList)
            if (loop.name == name)
                throw AlreadyRegistered("relic structure", name);

        namedRelicStructureList.emplace_back(name, structure);
        return *this;
    }

    ReliquaryOrigin& ReliquaryOrigin::CuratorPipeline(const Arca::Pipeline& pipeline)
    {
        curatorInitializationPipeline = pipeline;
        curatorWorkPipeline = pipeline;

        return *this;
    }

    ReliquaryOrigin& ReliquaryOrigin::CuratorPipeline(const Pipeline& initialization, const Pipeline& work)
    {
        curatorInitializationPipeline = initialization;
        curatorWorkPipeline = work;

        return *this;
    }

    ReliquaryOrigin::TypeConstructor::TypeConstructor(
        TypeHandleName typeHandleName, std::function<void(Reliquary&)>&& factory)
        :
        typeHandleName(std::move(typeHandleName)), factory(std::move(factory))
    {}

    std::vector<Curator*> ReliquaryOrigin::PushAllCuratorsTo(Reliquary& reliquary) const
    {
        std::vector<Arca::Curator*> returnValue;

        std::vector<CuratorProviderBase::Provided> provided;

        for (auto& loop : curatorProviders)
            provided.push_back(loop.second->Provide());

        for (auto& loop : provided)
        {
            Reliquary::CuratorHandlePtr handle;

            if (loop.curator.index() == 0)
                handle = std::make_unique<OwnedCuratorHandle>(std::move(std::get<0>(loop.curator)), loop.typeHandle);
            else
                handle = std::make_unique<UnownedCuratorHandle>(std::get<1>(loop.curator), loop.typeHandle);

            returnValue.push_back(handle->Get());

            reliquary.curators.emplace(handle->typeHandle.name, std::move(handle));
        }

        for (auto& loop : curatorSerializationTypeHandlesFactoryList)
            loop(reliquary);

        return returnValue;
    }

    Reliquary::CuratorPipeline ReliquaryOrigin::TransformCuratorPipeline(
        Reliquary& reliquary,
        const Pipeline& toTransform,
        const std::vector<Arca::Curator*>& allCurators)
    {
        auto pipeline = toTransform;
        for (auto loop = pipeline.begin(); loop != pipeline.end();)
        {
            if (loop->Empty())
                loop = pipeline.erase(loop);
            else
                ++loop;
        }

        Reliquary::CuratorPipeline createdPipeline;

        if (pipeline.empty())
        {
            createdPipeline.push_back(Reliquary::CuratorStage{});
            for (auto& loop : allCurators)
                createdPipeline.back().push_back(loop);

            return createdPipeline;
        }

        std::unordered_set<Arca::Curator*> seenCurators;

        for(auto& stage : toTransform)
        {
            Reliquary::CuratorStage createdStage;

            auto typeHandleNames = stage.TypeHandleNameList();
            for(auto& typeHandleName : typeHandleNames)
            {
                auto found = reliquary.Find<Arca::Curator>(typeHandleName);
                if (found == nullptr)
                    throw InvalidPipeline("Curator (" + typeHandleName + ") was not found.");

                const auto wasAlreadySeen = !seenCurators.emplace(found).second;
                if (wasAlreadySeen)
                    throw InvalidPipeline("Curator (" + typeHandleName + ") was already in the pipeline.");

                createdStage.push_back(found);
            }

            if (!createdStage.empty())
                createdPipeline.push_back(createdStage);
        }

        return createdPipeline;
    }

    ReliquaryOrigin::SignalConstructor::SignalConstructor(std::type_index type, void(*factory)(Reliquary&))
        : type(std::move(type)), factory(factory)
    {}
}