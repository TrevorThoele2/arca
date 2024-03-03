#include "ReliquaryOrigin.h"

#include <unordered_set>

#include "PipelineException.h"

namespace Arca
{
    std::unique_ptr<Reliquary> ReliquaryOrigin::Actualize()
    {
        ValidateCuratorPipeline(curatorConstructionPipeline);
        for (auto& commandPipeline : curatorCommandPipelines)
            ValidateCuratorPipeline(commandPipeline.second);

        auto reliquary = std::make_unique<Reliquary>();

        for (auto& initializer : shardList)
            initializer.factory(*reliquary);

        for (auto& initializer : relicList)
            initializer.factory(*reliquary);

        reliquary->relicStructures.namedList = namedRelicStructureList;

        for (auto& initializer : globalRelicList)
            initializer.factory(*reliquary);

        PushAllCuratorsTo(*reliquary);

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

    ReliquaryOrigin&& ReliquaryOrigin::CuratorConstructionPipeline(const Pipeline& pipeline)
    {
        curatorConstructionPipeline = pipeline;

        return std::move(*this);
    }

    ReliquaryOrigin&& ReliquaryOrigin::CuratorCommandPipeline(const TypeName& commandType, const Pipeline& commandPipeline)
    {
        curatorCommandPipelines.emplace(commandType, commandPipeline);

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

    void ReliquaryOrigin::PushAllCuratorsTo(Reliquary& reliquary)
    {
        std::unordered_set<TypeName> typesLeft;
        for (auto& initializer : curatorList)
            typesLeft.emplace(initializer.typeName);

        for(auto& stage : curatorConstructionPipeline)
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

        for(auto& curatorPipeline : curatorCommandPipelines)
        {
            const auto commandTypeName = curatorPipeline.first;
            auto& commandHandler = reliquary.commands.RequiredHandler(commandTypeName);
            commandHandler.SetupPipeline(curatorPipeline.second);
        }
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
                    throw NotRegistered("curator", Type{ typeName });
            }
        }
    }
}