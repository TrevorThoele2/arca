#include <catch.hpp>

#include "CuratorTests.h"

#include <Arca/PipelineException.h>
#include <Chroma/Iterate.h>

CuratorTestsFixture::CuratorCheckpoint::CuratorCheckpoint(Curator* curator, CuratorState state) :
    curator(curator), state(state)
{}

Reliquary& CuratorTestsFixture::BasicCurator::OwnerFromOutside()
{
    return Owner();
}

const Reliquary& CuratorTestsFixture::BasicCurator::OwnerFromOutside() const
{
    return Owner();
}

void CuratorTestsFixture::BasicCurator::InitializeImplementation()
{
    isInitialized = true;
}

bool CuratorTestsFixture::BasicCurator::StartStepImplementation()
{
    onStartStep();
    return shouldStart;
}

void CuratorTestsFixture::BasicCurator::WorkImplementation()
{
    onWork();
}

void CuratorTestsFixture::BasicCurator::StopStepImplementation()
{
    onStopStep();
}

namespace Arca
{
    template<>
    struct Traits<CuratorTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeHandleName typeName;
    };

    const TypeHandleName Traits<CuratorTestsFixture::BasicCurator>::typeName = "BasicCurator";

    template<>
    struct Traits<CuratorTestsFixture::OtherBasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeHandleName typeName;
    };

    const TypeHandleName Traits<CuratorTestsFixture::OtherBasicCurator>::typeName = "OtherBasicCurator";
}

SCENARIO_METHOD(CuratorTestsFixture, "curator", "[curator]")
{
    GIVEN("reliquary registered and initialized with curator without pipeline")
    {
        std::vector<CuratorState> states;

        auto reliquary = ReliquaryOrigin()
            .Type<BasicCurator>()
            .Actualize();

        auto curator = reliquary->Find<BasicCurator>();
        curator->onStartStep = [&states]()
        {
            states.push_back(CuratorState::Started);
        };
        curator->onWork = [&states]()
        {
            states.push_back(CuratorState::Worked);
        };
        curator->onStopStep = [&states]()
        {
            states.push_back(CuratorState::Stopped);
        };

        WHEN("checking reliquary curator size")
        {
            THEN("has one curator")
            {
                REQUIRE(reliquary->CuratorSize() == 1);
            }
        }

        WHEN("finding curator")
        {
            THEN("found is not null")
            {
                REQUIRE(curator != nullptr);
            }
        }

        WHEN("checking is initialized")
        {
            THEN("is initialized")
            {
                REQUIRE(curator->isInitialized == true);
            }
        }

        WHEN("default curator")
        {
            THEN("is not started")
            {
                REQUIRE(!curator->IsStarted());
            }

            THEN("owner is reliquary")
            {
                REQUIRE(&curator->OwnerFromOutside() == reliquary.get());
            }

            THEN("owner is reliquary const")
            {
                const auto constCurator = curator;
                REQUIRE(&constCurator->OwnerFromOutside() == reliquary.get());
            }
        }

        WHEN("starting curator")
        {
            curator->StartStep();

            THEN("starting curator again does not throw")
            {
                REQUIRE_NOTHROW(curator->StartStep());
            }
        }

        WHEN("working reliquary")
        {
            reliquary->Work();

            THEN("curator ran through states in order")
            {
                REQUIRE(states.size() == 3);
                REQUIRE(states[0] == CuratorState::Started);
                REQUIRE(states[1] == CuratorState::Worked);
                REQUIRE(states[2] == CuratorState::Stopped);
            }

            THEN("curator is not started")
            {
                REQUIRE(!curator->IsStarted());
            }
        }

        WHEN("curator should not be started")
        {
            curator->shouldStart = false;

            reliquary->Work();

            THEN("worked reliquary does not work curator")
            {
                REQUIRE(states.size() == 1);
                REQUIRE(states[0] == CuratorState::Started);
            }
        }
    }
}

template<size_t id>
struct PipelineIterator
{
    static void Do(Pipeline& pipeline)
    {
        pipeline.emplace_back();
        pipeline.back().Add<CuratorTestsFixture::DifferentiableCurator<id>>();
    }
};

template<size_t id>
struct ReliquaryOriginIterator
{
    static void Do(ReliquaryOrigin& reliquaryOrigin)
    {
        reliquaryOrigin.Type<CuratorTestsFixture::DifferentiableCurator<id>>();
    }
};

template<size_t id>
struct SetupDifferentiableCurator
{
    static void Do(std::vector<CuratorTestsFixture::CuratorCheckpoint>& checkpoints)
    {
        CuratorTestsFixture::DifferentiableCurator<id>::checkpoints = &checkpoints;
    }

    static void Do(
        std::vector<CuratorTestsFixture::CuratorCheckpoint>& checkpoints,
        std::vector<Curator*>& initializationCheckpoints)
    {
        CuratorTestsFixture::DifferentiableCurator<id>::checkpoints = &checkpoints;
        CuratorTestsFixture::DifferentiableCurator<id>::initializationCheckpoints = &initializationCheckpoints;
    }
};

template<size_t id>
struct RequireDifferentiableCuratorCheckpointVerification
{
    static void Do(
        std::vector<CuratorTestsFixture::CuratorCheckpoint>::iterator beginPosition,
        Reliquary& reliquary,
        std::list<bool>& output)
    {
        const auto curator = reliquary.Find<CuratorTestsFixture::DifferentiableCurator<id>>();

        const auto position = beginPosition + id;
        output.push_back(position->curator == curator);
    }

    static void Do(
        std::vector<Curator*>::iterator beginPosition,
        Reliquary& reliquary,
        std::list<bool>& output)
    {
        const auto curator = reliquary.Find<CuratorTestsFixture::DifferentiableCurator<id>>();

        const auto position = beginPosition + id;
        output.push_back(*position == curator);
    }
};

SCENARIO_METHOD(CuratorTestsFixture, "curator pipeline", "[curator][pipeline]")
{
    GIVEN("reliquary with pipeline with one hundred stages with one curator in each")
    {
        std::vector<CuratorCheckpoint> checkpoints;
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 99, 0>(checkpoints);

        auto pipeline = Pipeline();
        ::Chroma::IterateRange<size_t, PipelineIterator, 99, 0>(pipeline);

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline);
        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 99, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();

        WHEN("working reliquary")
        {
            reliquary->Work();

            THEN("executes all in order")
            {
                REQUIRE(checkpoints.size() == 300);

                std::list<bool> output;
                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (checkpoints.begin(), *reliquary, output);

                REQUIRE(output.size() == 100);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
                output.clear();

                REQUIRE(std::all_of(
                    checkpoints.begin(),
                    checkpoints.begin() + 100,
                    [](const CuratorCheckpoint& checkpoint) { return checkpoint.state == CuratorState::Started; }));

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (checkpoints.begin() + 100, *reliquary, output);

                REQUIRE(output.size() == 100);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
                output.clear();

                REQUIRE(std::all_of(
                    checkpoints.begin() + 100,
                    checkpoints.begin() + 200,
                    [](const CuratorCheckpoint& checkpoint) { return checkpoint.state == CuratorState::Worked; }));

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (checkpoints.begin() + 200, *reliquary, output);

                REQUIRE(output.size() == 100);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
                output.clear();

                REQUIRE(std::all_of(
                    checkpoints.begin() + 200,
                    checkpoints.end(),
                    [](const CuratorCheckpoint& checkpoint) { return checkpoint.state == CuratorState::Stopped; }));
            }
        }
    }

    GIVEN("reliquary origin with pipeline that contains nonexistent curator")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();
        pipeline.back().Add<BasicCurator>();

        const auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline);

        WHEN("actualized")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    InvalidPipeline,
                    ::Catch::Matchers::Message("Curator (" + Traits<BasicCurator>::typeName + ") was not found.")
                );
            }
        }
    }

    GIVEN("reliquary origin with pipeline that contains curator multiple times")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();
        pipeline.back().Add<BasicCurator>();
        pipeline.back().Add<BasicCurator>();

        const auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline)
            .Type<BasicCurator>();

        WHEN("actualized")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    InvalidPipeline,
                    ::Catch::Matchers::Message("Curator (" + Traits<BasicCurator>::typeName + ") " +
                        "was already in the pipeline.")
                );
            }
        }
    }

    GIVEN("reliquary origin with pipeline that contains only empty stage")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();

        const auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline);

        WHEN("actualized")
        {
            THEN("no throw")
            {
                REQUIRE_NOTHROW(reliquaryOrigin.Actualize());
            }
        }
    }
}

SCENARIO_METHOD(CuratorTestsFixture, "curator split pipeline", "[curator][pipeline]")
{
    GIVEN("reliquary with pipeline with one hundred stages with one curator in each")
    {
        std::vector<CuratorCheckpoint> checkpoints;
        std::vector<Curator*> initializationCheckpoints;
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 99, 0>(checkpoints, initializationCheckpoints);

        auto initializationPipeline = Pipeline();
        ::Chroma::IterateRangeBackward<size_t, PipelineIterator, 99, 0>(initializationPipeline);
        auto workPipeline = Pipeline();
        ::Chroma::IterateRange<size_t, PipelineIterator, 99, 0>(workPipeline);

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(initializationPipeline, workPipeline);
        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 99, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();

        THEN("executed initialization in backwards order")
        {
            std::list<bool> output;
            ::Chroma::IterateRangeBackward<
                size_t,
                RequireDifferentiableCuratorCheckpointVerification,
                99,
                0
            >
                (initializationCheckpoints.begin(), *reliquary, output);
        }

        WHEN("working reliquary")
        {
            reliquary->Work();

            THEN("executes all in order")
            {
                REQUIRE(checkpoints.size() == 300);

                std::list<bool> output;
                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (checkpoints.begin(), *reliquary, output);

                REQUIRE(output.size() == 100);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
                output.clear();

                REQUIRE(std::all_of(
                    checkpoints.begin(),
                    checkpoints.begin() + 100,
                    [](const CuratorCheckpoint& checkpoint) { return checkpoint.state == CuratorState::Started; }));

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (checkpoints.begin() + 100, *reliquary, output);

                REQUIRE(output.size() == 100);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
                output.clear();

                REQUIRE(std::all_of(
                    checkpoints.begin() + 100,
                    checkpoints.begin() + 200,
                    [](const CuratorCheckpoint& checkpoint) { return checkpoint.state == CuratorState::Worked; }));

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (checkpoints.begin() + 200, *reliquary, output);

                REQUIRE(output.size() == 100);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
                output.clear();

                REQUIRE(std::all_of(
                    checkpoints.begin() + 200,
                    checkpoints.end(),
                    [](const CuratorCheckpoint& checkpoint) { return checkpoint.state == CuratorState::Stopped; }));
            }
        }
    }

    GIVEN("reliquary origin with pipeline that contains nonexistent curator")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();
        pipeline.back().Add<BasicCurator>();

        const auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline);

        WHEN("actualized")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    InvalidPipeline,
                    ::Catch::Matchers::Message("Curator (" + Traits<BasicCurator>::typeName + ") was not found.")
                );
            }
        }
    }

    GIVEN("reliquary origin with pipeline that contains curator multiple times")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();
        pipeline.back().Add<BasicCurator>();
        pipeline.back().Add<BasicCurator>();

        const auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline)
            .Type<BasicCurator>();

        WHEN("actualized")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    InvalidPipeline,
                    ::Catch::Matchers::Message("Curator (" + Traits<BasicCurator>::typeName + ") " +
                        "was already in the pipeline.")
                );
            }
        }
    }

    GIVEN("reliquary origin with pipeline that contains only empty stage")
    {
        auto pipeline = Pipeline();
        pipeline.emplace_back();

        const auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline);

        WHEN("actualized")
        {
            THEN("no throw")
            {
                REQUIRE_NOTHROW(reliquaryOrigin.Actualize());
            }
        }
    }
}

SCENARIO_METHOD(CuratorTestsFixture, "curator serialization", "[curator][serialization]")
{
    GIVEN("saved reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Type<BasicCurator>()
            .Actualize();

        auto savedCurator = savedReliquary->Find<BasicCurator>();
        savedCurator->value = dataGeneration.Random<int>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicCurator>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedCurator = loadedReliquary->Find<BasicCurator>();

            THEN("loaded curator is not null")
            {
                REQUIRE(loadedCurator != nullptr);
            }

            THEN("value is loaded")
            {
                REQUIRE(loadedCurator->value == savedCurator->value);
            }
        }

        WHEN("loading reliquary without curator registered")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedCurator = loadedReliquary->Find<BasicCurator>();

            THEN("loaded curator is null")
            {
                REQUIRE(loadedCurator == nullptr);
            }
        }

        WHEN("loading reliquary with different curator with same input type handle")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<OtherBasicCurator>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");;
                inputArchive(*loadedReliquary);
            }

            auto loadedCurator = loadedReliquary->Find<OtherBasicCurator>();

            THEN("loaded curator is not null")
            {
                REQUIRE(loadedCurator != nullptr);
            }

            THEN("value is loaded")
            {
                REQUIRE(loadedCurator->value == savedCurator->value);
            }
        }
    }
}

namespace Inscription
{
    void Scribe<CuratorTestsFixture::BasicCurator, BinaryArchive>::ScrivenImplementation(
        ObjectT& object, ArchiveT& archive)
    {
        archive(object.value);
    }

    void Scribe<CuratorTestsFixture::OtherBasicCurator, BinaryArchive>::ScrivenImplementation(
        ObjectT& object, ArchiveT& archive)
    {
        archive(object.value);
    }

    std::vector<TypeHandle> Scribe<CuratorTestsFixture::OtherBasicCurator, BinaryArchive>::InputTypeHandles(const ArchiveT& archive)
    {
        return { "BasicCurator" };
    }
}