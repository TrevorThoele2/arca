#include <catch.hpp>

#include "CuratorTests.h"

#include "DifferentiableCurator.h"

#include <Arca/PipelineException.h>
#include <Chroma/Iterate.h>

void CuratorTestsFixture::BasicCurator::Handle(const BasicCommand& command)
{
    onCommand(command);
}

Reliquary& CuratorTestsFixture::BasicCurator::OwnerFromOutside()
{
    return Owner();
}

const Reliquary& CuratorTestsFixture::BasicCurator::OwnerFromOutside() const
{
    return Owner();
}

CuratorTestsFixture::CuratorWithNonDefaultConstructor::CuratorWithNonDefaultConstructor(
    Init init, int myValue)
    :
    Curator(init), myValue(myValue)
{}

CuratorTestsFixture::CuratorWithLocalRelicConstructor::CuratorWithLocalRelicConstructor(
    Init init, int localRelicInteger, const std::string& localRelicString)
    :
    Curator(init),
    localRelic(init.owner.Do(Create<BasicLocalClosedTypedRelic>{localRelicInteger, localRelicString}))
{
    this->localRelicInteger = localRelic->integer;
    this->localRelicString = localRelic->string;
}

CuratorTestsFixture::CuratorWithGlobalRelicConstructor::CuratorWithGlobalRelicConstructor(Init init) :
    Curator(init), globalRelic(init.owner.Find<BasicGlobalClosedTypedRelic>())
{
    globalRelicInteger = globalRelic->integer;
    globalRelicString = globalRelic->string;
}

CuratorTestsFixture::CuratorWithoutCommands::CuratorWithoutCommands(Init init) : Curator(init)
{}

CuratorTestsFixture::BaseCuratorWithCommand::BaseCuratorWithCommand(Init init) : Curator(init)
{}

void CuratorTestsFixture::BaseCuratorWithCommand::Handle(const BasicCommand& command)
{
    basicCommandIssued = true;
}

CuratorTestsFixture::DerivedCuratorWithCommand::DerivedCuratorWithCommand(Init init) : BaseCuratorWithCommand(init)
{}

void CuratorTestsFixture::DerivedCuratorWithCommand::Handle(const BasicCommand2& command)
{}

template<size_t id>
struct StagePerCuratorIterator
{
    static void Do(Pipeline& pipeline)
    {
        pipeline.emplace_back();
        pipeline.back().Add<DifferentiableCurator<id>>();
    }
};

template<size_t id>
struct StageIterator
{
    static void Do(Stage& stage)
    {
        stage.Add<DifferentiableCurator<id>>();
    }
};

template<size_t id>
struct ReliquaryOriginIterator
{
    static void Do(ReliquaryOrigin& reliquaryOrigin)
    {
        reliquaryOrigin.Register<DifferentiableCurator<id>>();
    }
};

template<size_t id>
struct SetupDifferentiableCurator
{
    static void Do(std::vector<Curator*>& commandCheckpoints)
    {
        DifferentiableCurator<id>::onConstructor = [&commandCheckpoints](DifferentiableCurator<id>& curator)
        {
            commandCheckpoints.push_back(&curator);
        };
    }

    static void Do(Reliquary& reliquary, std::vector<Curator*>& commandCheckpoints)
    {
        reliquary.Find<DifferentiableCurator<id>>().onCommand = [&commandCheckpoints](DifferentiableCuratorBase& curator)
        {
            commandCheckpoints.push_back(&curator);
        };
    }
};

template<size_t id>
struct RequireDifferentiableCuratorCheckpointVerification
{
    static void Do(
        std::vector<Curator*>& curators,
        Reliquary& reliquary,
        std::list<bool>& output)
    {
        if (curators.size() < id + 1)
            return;

        const auto& curator = reliquary.Find<DifferentiableCurator<id>>();

        output.push_back(curators[id] == &curator);
    }
};

SCENARIO_METHOD(CuratorTestsFixture, "curator", "[curator]")
{
    GIVEN("reliquary registered and initialized with curator without pipeline")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<BasicCurator>()
            .Actualize();

        auto& curator = reliquary->Find<BasicCurator>();

        WHEN("checking reliquary curator size")
        {
            THEN("has one curator")
            {
                REQUIRE(reliquary->CuratorSize() == 1);
            }
        }

        WHEN("default curator")
        {
            THEN("owner is reliquary")
            {
                REQUIRE(&curator.OwnerFromOutside() == reliquary.get());
            }

            THEN("owner is reliquary const")
            {
                const auto& constCurator = curator;
                REQUIRE(&constCurator.OwnerFromOutside() == reliquary.get());
            }
        }
    }
}

SCENARIO_METHOD(CuratorTestsFixture, "curator constructor", "[curator]")
{
    GIVEN("reliquary registered")
    {
        auto localRelicInteger = dataGeneration.Random<int>();
        auto localRelicString = dataGeneration.Random<std::string>();
        auto globalRelicInteger = dataGeneration.Random<int>();
        auto globalRelicString = dataGeneration.Random<std::string>();

        auto reliquary = ReliquaryOrigin()
            .Register<BasicLocalClosedTypedRelic>()
            .Register<BasicGlobalClosedTypedRelic>(globalRelicInteger, globalRelicString)
            .Register<CuratorWithLocalRelicConstructor>(localRelicInteger, localRelicString)
            .Register<CuratorWithGlobalRelicConstructor>()
            .Actualize();

        WHEN("querying curator with local relic's value")
        {
            auto& curator = reliquary->Find<CuratorWithLocalRelicConstructor>();
            auto localRelic = curator.localRelic;

            THEN("is occupied")
            {
                REQUIRE(localRelic);
            }

            THEN("curator has values")
            {
                REQUIRE(curator.localRelicInteger == localRelicInteger);
                REQUIRE(curator.localRelicString == localRelicString);
            }
        }

        WHEN("querying curator with global relic's value")
        {
            auto& curator = reliquary->Find<CuratorWithGlobalRelicConstructor>();
            auto globalRelic = curator.globalRelic;

            THEN("is occupied")
            {
                REQUIRE(globalRelic);
            }

            THEN("curator has values")
            {
                REQUIRE(curator.globalRelicInteger == globalRelicInteger);
                REQUIRE(curator.globalRelicString == globalRelicString);
            }
        }
    }
}

SCENARIO_METHOD(CuratorTestsFixture, "curator pipeline", "[curator][pipeline]")
{
    GIVEN("reliquary with pipeline with one hundred stages with one curator in each")
    {
        std::vector<Curator*> constructorCheckpoints;
        std::vector<Curator*> commandCheckpoints;
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 99, 0>(constructorCheckpoints);

        auto constructorPipeline = Pipeline();
        ::Chroma::IterateRangeBackward<size_t, StagePerCuratorIterator, 99, 0>(constructorPipeline);
        auto commandPipeline = Pipeline();
        ::Chroma::IterateRange<size_t, StagePerCuratorIterator, 99, 0>(commandPipeline);

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorConstructionPipeline(constructorPipeline)
            .CuratorCommandPipeline(TypeFor<BasicCommand>().name, commandPipeline);
        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 99, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 99, 0>(*reliquary, commandCheckpoints);

        THEN("executed construction in backwards order")
        {
            std::list<bool> output;
            ::Chroma::IterateRangeBackward<
                size_t,
                RequireDifferentiableCuratorCheckpointVerification,
                99,
                0
            >
                (constructorCheckpoints, *reliquary, output);
        }

        WHEN("sending command")
        {
            reliquary->Do(BasicCommand{});

            THEN("executes all in order")
            {
                REQUIRE(commandCheckpoints.size() == 100);

                std::list<bool> output;

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (commandCheckpoints, *reliquary, output);

                REQUIRE(output.size() == 100);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
            }
        }
    }

    GIVEN("reliquary origin with construction pipeline that contains nonexistent curator")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();
        pipeline.back().Add<BasicCurator>();

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorConstructionPipeline(pipeline);

        WHEN("actualized")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquaryOrigin.Actualize(), NotRegistered);
            }
        }
    }

    GIVEN("reliquary origin with command pipeline that contains nonexistent curator")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();
        pipeline.back().Add<BasicCurator>();

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorCommandPipeline<BasicCommand>(pipeline);

        WHEN("actualized")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquaryOrigin.Actualize(), NotRegistered);
            }
        }
    }

    GIVEN("reliquary origin with construction pipeline that contains curator multiple times")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();
        pipeline.back().Add<BasicCurator>();
        pipeline.back().Add<BasicCurator>();

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorConstructionPipeline(pipeline)
            .Register<BasicCurator>();

        WHEN("actualized")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    InvalidPipeline,
                    ::Catch::Matchers::Message("Curator (" + Traits<BasicCurator>::TypeName() + ") " +
                        "was already in the pipeline.")
                );
            }
        }
    }

    GIVEN("reliquary origin with command pipeline that contains curator multiple times")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();
        pipeline.back().Add<BasicCurator>();
        pipeline.back().Add<BasicCurator>();

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorCommandPipeline<BasicCommand>(pipeline)
            .Register<BasicCurator>();

        WHEN("actualized")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    InvalidPipeline,
                    ::Catch::Matchers::Message("Curator (" + Traits<BasicCurator>::TypeName() + ") " +
                        "was already in the pipeline.")
                );
            }
        }
    }

    GIVEN("reliquary origin with construction pipeline that contains only empty stage")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorConstructionPipeline(pipeline);

        WHEN("actualized")
        {
            THEN("no throw")
            {
                REQUIRE_NOTHROW(reliquaryOrigin.Actualize());
            }
        }
    }

    GIVEN("reliquary origin with command pipeline that contains only empty stage")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorCommandPipeline<BasicCommand>(pipeline)
            .Register<DifferentiableCurator<0>>();

        WHEN("actualized")
        {
            THEN("no throw")
            {
                REQUIRE_NOTHROW(reliquaryOrigin.Actualize());
            }
        }
    }

    GIVEN("reliquary origin with command pipeline that contains multiple stages")
    {
        std::vector<Curator*> commandCheckpoints;

        auto commandPipeline = Pipeline();
        commandPipeline.emplace_back();
        commandPipeline.back().Add<DifferentiableCurator<0>>();
        commandPipeline.emplace_back();
        commandPipeline.back().Add<DifferentiableCurator<1>>();
        commandPipeline.back().Add<DifferentiableCurator<2>>();
        commandPipeline.emplace_back();
        commandPipeline.back().Add<DifferentiableCurator<3>>();

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorCommandPipeline<BasicCommand>(commandPipeline);

        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 3, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();

        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 3, 0>(*reliquary, commandCheckpoints);

        WHEN("sending command")
        {
            reliquary->Do(BasicCommand{});

            THEN("executed first curator then second curator")
            {
                REQUIRE(commandCheckpoints.size() == 4);

                REQUIRE(commandCheckpoints[0] == &reliquary->Find<DifferentiableCurator<0>>());
                REQUIRE(commandCheckpoints[1] == &reliquary->Find<DifferentiableCurator<1>>());
                REQUIRE(commandCheckpoints[2] == &reliquary->Find<DifferentiableCurator<2>>());
                REQUIRE(commandCheckpoints[3] == &reliquary->Find<DifferentiableCurator<3>>());
            }
        }
    }

    GIVEN("reliquary origin with command pipeline that contains curators with command and without command")
    {
        std::vector<Curator*> commandCheckpoints;

        auto commandPipeline = Pipeline();
        commandPipeline.emplace_back();
        commandPipeline.back().Add<DifferentiableCurator<0>>();
        commandPipeline.back().Add<CuratorWithoutCommands>();

        auto reliquaryOrigin = ReliquaryOrigin()
            .Register<CuratorWithoutCommands>()
            .CuratorCommandPipeline<BasicCommand>(commandPipeline);

        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 0, 0>(reliquaryOrigin);

        WHEN("actualizing")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    Arca::CommandNotRegisteredInCurator,
                    ::Catch::Matchers::Message(
                        "The command (" + Traits<BasicCommand>::TypeName() + ") has not been linked in the curator (" +
                        Traits<CuratorWithoutCommands>::TypeName() + ").")
                );
            }
        }
    }
}

SCENARIO_METHOD(CuratorTestsFixture, "curator aborts pipeline", "[curator][pipeline]")
{
    GIVEN("reliquary with pipeline with one hundred stages with one curator in each")
    {
        std::vector<Curator*> constructorCheckpoints;
        std::vector<Curator*> commandCheckpoints;
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 99, 0>(constructorCheckpoints);

        auto pipeline = Pipeline();
        ::Chroma::IterateRange<size_t, StagePerCuratorIterator, 99, 0>(pipeline);

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorConstructionPipeline(pipeline)
            .CuratorCommandPipeline<BasicCommand>(pipeline);
        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 99, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 99, 0>(*reliquary, commandCheckpoints);

        WHEN("sending command when middle curator will abort")
        {
            auto& middleCurator = reliquary->Find<DifferentiableCurator<49>>();
            middleCurator.shouldAbort = true;

            reliquary->Do(BasicCommand{});

            THEN("executed up until 51st")
            {
                REQUIRE(commandCheckpoints.size() == 50);

                std::list<bool> output;

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (commandCheckpoints, *reliquary, output);

                REQUIRE(output.size() == 50);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
            }
        }
    }

    GIVEN("nine curators in three stages")
    {
        std::vector<Curator*> constructorCheckpoints;
        std::vector<Curator*> commandCheckpoints;
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 8, 0>(constructorCheckpoints);

        auto pipeline = Pipeline();
        pipeline.resize(3);
        auto& stage1 = pipeline[0];
        auto& stage2 = pipeline[1];
        auto& stage3 = pipeline[2];
        ::Chroma::IterateRange<size_t, StageIterator, 2, 0>(stage1);
        ::Chroma::IterateRange<size_t, StageIterator, 5, 3>(stage2);
        ::Chroma::IterateRange<size_t, StageIterator, 8, 6>(stage3);

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorConstructionPipeline(pipeline)
            .CuratorCommandPipeline<BasicCommand>(pipeline);
        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 8, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 8, 0>(*reliquary, commandCheckpoints);

        WHEN("sending command when middle stage will abort")
        {
            auto& middleCurator = reliquary->Find<DifferentiableCurator<4>>();
            middleCurator.shouldAbort = true;

            reliquary->Do(BasicCommand{});

            THEN("executed all up to and including aborting curator")
            {
                REQUIRE(commandCheckpoints.size() == 5);

                std::list<bool> output;

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    8,
                    0
                >
                    (commandCheckpoints, *reliquary, output);

                auto outputMiddle = std::next(output.begin(), 5);

                REQUIRE(output.size() == 5);
                REQUIRE(std::all_of(
                    output.begin(),
                    outputMiddle,
                    [](const bool& entry) { return entry; }));
                REQUIRE(std::all_of(
                    outputMiddle,
                    output.end(),
                    [](const bool& entry) { return !entry; }));
            }
        }
    }
}

SCENARIO_METHOD(CuratorTestsFixture, "non default curator construction", "[curator]")
{
    GIVEN("registered reliquary")
    {
        auto myValue = dataGeneration.Random<int>();

        auto reliquary = ReliquaryOrigin()
            .Register<CuratorWithNonDefaultConstructor>(myValue)
            .Actualize();

        WHEN("retrieving curator")
        {
            auto& curator = reliquary->Find<CuratorWithNonDefaultConstructor>();

            THEN("has value given to constructor")
            {
                REQUIRE(curator.myValue == myValue);
            }
        }
    }
}

SCENARIO_METHOD(CuratorTestsFixture, "curator command from base class", "[curator][command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<DerivedCuratorWithCommand>()
            .Actualize();

        auto& derivedCurator = reliquary->Find<DerivedCuratorWithCommand>();

        WHEN("issuing command")
        {
            THEN("value is loaded")
            {
                reliquary->Do(BasicCommand{});

                REQUIRE(derivedCurator.basicCommandIssued);
            }
        }
    }
}

SCENARIO_METHOD(CuratorTestsFixture, "curator serialization", "[curator][serialization]")
{
    GIVEN("saved reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicCurator>()
            .Actualize();
    
        auto& savedCurator = savedReliquary->Find<BasicCurator>();
        savedCurator.value = dataGeneration.Random<int>();
    
        {
            auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
            outputArchive(*savedReliquary);
        }
    
        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicCurator>()
                .Actualize();
        
            {
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
            }
        
            auto& loadedCurator = loadedReliquary->Find<BasicCurator>();
        
            THEN("value is loaded")
            {
                REQUIRE(loadedCurator.value == savedCurator.value);
            }
        }
    
        WHEN("loading reliquary without curator registered")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Actualize();
        
            {
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
            }
        
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(loadedReliquary->Find<BasicCurator>(), NotRegistered);
            }
        }
    
        WHEN("loading reliquary with different curator with same input type handle")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OtherBasicCurator>()
                .Actualize();
        
            {
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");;
                inputArchive(*loadedReliquary);
            }
        
            auto& loadedCurator = loadedReliquary->Find<OtherBasicCurator>();
        
            THEN("value is loaded")
            {
                REQUIRE(loadedCurator.value == savedCurator.value);
            }
        }
    }

    GIVEN("saved reliquary with local and global")
    {
        auto localRelicInteger = dataGeneration.Random<int>();
        auto localRelicString = dataGeneration.Random<std::string>();
        auto globalRelicInteger = dataGeneration.Random<int>();
        auto globalRelicString = dataGeneration.Random<std::string>();

        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicLocalClosedTypedRelic>()
            .Register<BasicGlobalClosedTypedRelic>(globalRelicInteger, globalRelicString)
            .Register<CuratorWithLocalRelicConstructor>(localRelicInteger, localRelicString)
            .Register<CuratorWithGlobalRelicConstructor>()
            .Actualize();

        {
            auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto localRelicInteger2 = dataGeneration.Random<int>();
            auto localRelicString2 = dataGeneration.Random<std::string>();
            auto globalRelicInteger2 = dataGeneration.Random<int>();
            auto globalRelicString2 = dataGeneration.Random<std::string>();

            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicLocalClosedTypedRelic>()
                .Register<BasicGlobalClosedTypedRelic>(globalRelicInteger2, globalRelicString2)
                .Register<CuratorWithLocalRelicConstructor>(localRelicInteger2, localRelicString2)
                .Register<CuratorWithGlobalRelicConstructor>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
            }

            WHEN("querying curator with local relic's value")
            {
                auto& curator = loadedReliquary->Find<CuratorWithLocalRelicConstructor>();
                auto localRelic = curator.localRelic;

                THEN("is occupied")
                {
                    REQUIRE(localRelic);
                }

                THEN("curator has values")
                {
                    REQUIRE(curator.localRelicInteger == localRelicInteger);
                    REQUIRE(curator.localRelicString == localRelicString);
                }
            }

            WHEN("querying curator with global relic's value")
            {
                auto& curator = loadedReliquary->Find<CuratorWithGlobalRelicConstructor>();
                auto globalRelic = curator.globalRelic;

                THEN("is occupied")
                {
                    REQUIRE(globalRelic);
                }

                THEN("curator has values")
                {
                    REQUIRE(curator.globalRelicInteger == globalRelicInteger);
                    REQUIRE(curator.globalRelicString == globalRelicString);
                }
            }
        }
    }
}