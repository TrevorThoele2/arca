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

    std::unique_ptr<Reliquary> ReliquaryOrigin::Actualize()
    {
        ValidateCuratorPipeline(curatorConstructionPipeline);
        ValidateCuratorPipeline(curatorWorkPipeline);

        auto reliquary = std::make_unique<Reliquary>();

        for (auto& initializer : shardList)
            initializer.factory(*reliquary);

        for (auto& initializer : relicList)
            initializer.factory(*reliquary);

        reliquary->relicStructures.namedList = namedRelicStructureList;

        for (auto& initializer : globalRelicList)
            initializer.factory(*reliquary);

        PushAllCuratorsTo(*reliquary, curatorConstructionPipeline);

        reliquary->curators.workPipeline = TransformCuratorPipeline(*reliquary, curatorWorkPipeline);

        return reliquary;
    }

    ReliquaryOrigin&& ReliquaryOrigin::RelicStructure(const std::string& name, const Arca::RelicStructure& structure)
    {
        for (auto& loop : namedRelicStructureList)
            if (loop.name == name)
                throw AlreadyRegistered("relic structure", Arca::Type(name));

        namedRelicStructureList.emplace_back(name, structure);
        return std::move(*this);
    }

    ReliquaryOrigin&& ReliquaryOrigin::CuratorPipeline(const Arca::Pipeline& pipeline)
    {
        curatorConstructionPipeline = pipeline;
        curatorWorkPipeline = pipeline;

        return std::move(*this);
    }

    ReliquaryOrigin&& ReliquaryOrigin::CuratorPipeline(const Pipeline& construction, const Pipeline& work)
    {
        curatorConstructionPipeline = construction;
        curatorWorkPipeline = work;

        return std::move(*this);
    }

    ReliquaryOrigin::TypeConstructor::TypeConstructor(TypeName typeName, Factory&& factory) :
        typeName(std::move(typeName)), factory(std::move(factory))
    {}

    ReliquaryOrigin::TypeConstructor::TypeConstructor(TypeConstructor && arg) noexcept :
        typeName(std::move(arg.typeName)), factory(std::move(arg.factory))
    {}

    auto ReliquaryOrigin::TypeConstructor::operator=(TypeConstructor && arg) noexcept -> TypeConstructor&
    {
        typeName = std::move(arg.typeName);
        factory = std::move(arg.factory);
        return *this;
    }

    void ReliquaryOrigin::PushAllCuratorsTo(Reliquary& reliquary, const Pipeline& pipeline)
    {
        std::unordered_set<TypeName> typesLeft;
        for (auto& initializer : curatorList)
            typesLeft.emplace(initializer.typeName);

        for(auto& stage : pipeline)
        {
            for(auto& typeName : stage.TypeNameList())
            {
                for(auto& initializer : curatorList)
                {
                    if (typeName == initializer.typeName)
                    {
                        initializer.factory(reliquary);
                        typesLeft.erase(typeName);
                    }
                }
            }
        }

        for(auto& typeLeft : typesLeft)
            for(auto& initializer : curatorList)
                if (typeLeft == initializer.typeName)
                    initializer.factory(reliquary);
    }

    void ReliquaryOrigin::ValidateCuratorPipeline(const Pipeline& pipeline) const
    {
        std::unordered_set<TypeName> seenTypes;
        for (auto& stage : pipeline)
        {
            for (auto& typeName : stage.TypeNameList())
            {
                if (seenTypes.find(typeName) != seenTypes.end())
                    throw InvalidPipeline("Curator (" + typeName + ") was already in the pipeline.");
                else
                    seenTypes.emplace(typeName);

                auto foundInitializer = false;
                for (auto& initializer : curatorList)
                {
                    if (typeName == initializer.typeName)
                    {
                        foundInitializer = true;
                        break;
                    }
                }

                if (!foundInitializer)
                    throw NotRegistered("Curator", Type{ typeName });
            }
        }
    }

    ReliquaryCurators::Pipeline ReliquaryOrigin::TransformCuratorPipeline(
        Reliquary& reliquary,
        const Pipeline& toTransform)
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
            for (auto& loop : reliquary.curators.handlers)
                createdPipeline.back().push_back(loop.get());

            return createdPipeline;
        }
        else
        {
            for (auto& stage : toTransform)
            {
                ReliquaryCurators::Stage createdStage;

                for (auto& typeName : stage.TypeNameList())
                {
                    auto found = std::find_if(
                        reliquary.curators.handlers.begin(),
                        reliquary.curators.handlers.end(),
                        [typeName](const ReliquaryCurators::HandlerPtr& ptr)
                        {
                            return typeName == ptr->typeName;
                        });
                    if (found == reliquary.curators.handlers.end())
                        continue;

                    createdStage.push_back(found->get());
                }

                if (!createdStage.empty())
                    createdPipeline.push_back(createdStage);
            }

            return createdPipeline;
        }
    }
}