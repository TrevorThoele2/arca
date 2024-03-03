#include "ReliquaryOrigin.h"

#include <unordered_set>

#include "PipelineException.h"

namespace Arca
{
    ReliquaryOrigin::ReliquaryOrigin()
    {
        Register<OpenRelic>();
        Register<ClosedRelic>();
    }

    ReliquaryOrigin::ReliquaryOrigin(const ReliquaryOrigin& arg) :
        relicList(arg.relicList),
        globalRelicList(arg.globalRelicList),
        namedRelicStructureList(arg.namedRelicStructureList),
        shardList(arg.shardList),
        curatorList(arg.curatorList),
        curatorInitializationPipeline(arg.curatorInitializationPipeline),
        curatorWorkPipeline(arg.curatorWorkPipeline)
    {}

    ReliquaryOrigin& ReliquaryOrigin::operator=(const ReliquaryOrigin& arg)
    {
        relicList = arg.relicList;
        globalRelicList = arg.globalRelicList;
        namedRelicStructureList = arg.namedRelicStructureList;
        shardList = arg.shardList;
        curatorList = arg.curatorList;
        curatorInitializationPipeline = arg.curatorInitializationPipeline;
        curatorWorkPipeline = arg.curatorWorkPipeline;

        return *this;
    }

    std::unique_ptr<Reliquary> ReliquaryOrigin::Actualize() const
    {
        auto reliquary = std::make_unique<Reliquary>();

        reliquary->signals.batchSources.transferableSignals.emplace();

        for (auto& initializer : shardList)
            initializer.factory(*reliquary);

        for (auto& initializer : relicList)
            initializer.factory(*reliquary);

        reliquary->relicStructures.namedList = namedRelicStructureList;

        const auto allCurators = PushAllCuratorsTo(*reliquary);

        auto transformedCuratorInitializationPipeline =
            TransformCuratorPipeline(
                *reliquary,
                curatorInitializationPipeline,
                allCurators);
        reliquary->curators.workPipeline =
            TransformCuratorPipeline(
                *reliquary,
                curatorWorkPipeline,
                allCurators);

        for (auto& initializer : globalRelicList)
            initializer.factory(*reliquary);

        for (auto& initializer : globalComputationInitializerMap)
            initializer.second(*reliquary);

        {
            for (auto& stage : transformedCuratorInitializationPipeline)
                for (auto& curator : stage)
                    curator->PostConstruct(*reliquary);

            for (auto& stage : transformedCuratorInitializationPipeline)
                for (auto& curator : stage)
                    curator->Initialize();
        }

        return reliquary;
    }

    ReliquaryOrigin& ReliquaryOrigin::RelicStructure(const std::string& name, const Arca::RelicStructure& structure)
    {
        for (auto& loop : namedRelicStructureList)
            if (loop.name == name)
                throw AlreadyRegistered("relic structure", Arca::Type(name));

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
        TypeName typeName, std::function<void(Reliquary&)>&& factory)
        :
        typeName(std::move(typeName)), factory(std::move(factory))
    {}

    std::vector<Curator*> ReliquaryOrigin::PushAllCuratorsTo(Reliquary& reliquary) const
    {
        for (auto& initializer : curatorList)
            initializer.factory(reliquary);

        std::vector<Curator*> returnValue;
        for (auto& loop : reliquary.curators.handlers)
            returnValue.push_back(&loop->Value());
        return returnValue;
    }

    ReliquaryCurators::Pipeline ReliquaryOrigin::TransformCuratorPipeline(
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

        ReliquaryCurators::Pipeline createdPipeline;

        if (pipeline.empty())
        {
            createdPipeline.push_back(ReliquaryCurators::Stage{});
            for (auto& loop : allCurators)
                createdPipeline.back().push_back(loop);

            return createdPipeline;
        }

        std::unordered_set<Arca::Curator*> seenCurators;

        for(auto& stage : toTransform)
        {
            ReliquaryCurators::Stage createdStage;

            auto typeNames = stage.TypeNameList();
            for(auto& typeName : typeNames)
            {
                auto& found = reliquary.Find<Arca::Curator>(typeName);

                const auto wasAlreadySeen = !seenCurators.emplace(&found).second;
                if (wasAlreadySeen)
                    throw InvalidPipeline("Curator (" + typeName + ") was already in the pipeline.");

                createdStage.push_back(&found);
            }

            if (!createdStage.empty())
                createdPipeline.push_back(createdStage);
        }

        return createdPipeline;
    }
}