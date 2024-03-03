#include <catch.hpp>

#include "CuratorTests.h"

#include <Arca/PipelineException.h>
#include <Chroma/Iterate.h>

CuratorTestsFixture::CuratorCheckpoint::CuratorCheckpoint(Curator* curator) : curator(curator)
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

void CuratorTestsFixture::BasicCurator::WorkImplementation(Stage& stage)
{
    if (shouldAbort)
    {
        stage.Abort();
        return;
    }

    onWork();
}

namespace Arca
{
    template<>
    struct Traits<CuratorTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeName typeName;
    };

    const TypeName Traits<CuratorTestsFixture::BasicCurator>::typeName = "BasicCurator";

    template<>
    struct Traits<CuratorTestsFixture::OtherBasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeName typeName;
    };

    const TypeName Traits<CuratorTestsFixture::OtherBasicCurator>::typeName = "OtherBasicCurator";
}

SCENARIO_METHOD(CuratorTestsFixture, "curator", "[curator]")
{
    GIVEN("reliquary registered and initialized with curator without pipeline")
    {
        std::vector<Curator*> worked;

        auto reliquary = ReliquaryOrigin()
            .Type<BasicCurator>()
            .Actualize();

        auto& curator = reliquary->Find<BasicCurator>();
        curator.onWork = [&curator, &worked]()
        {
            worked.push_back(&curator);
        };

        WHEN("checking reliquary curator size")
        {
            THEN("has one curator")
            {
                REQUIRE(reliquary->CuratorSize() == 1);
            }
        }

        WHEN("checking is initialized")
        {
            THEN("is initialized")
            {
                REQUIRE(curator.isInitialized == true);
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

        WHEN("working reliquary")
        {
            reliquary->Work();

            THEN("curator was worked")
            {
                REQUIRE(worked.size() == 1);
            }
        }
    }
}

template<size_t id>
struct StagePerCuratorIterator
{
    static void Do(Pipeline& pipeline)
    {
        pipeline.emplace_back();
        pipeline.back().Add<CuratorTestsFixture::DifferentiableCurator<id>>();
    }
};

template<size_t id>
struct StageIterator
{
    static void Do(Stage& stage)
    {
        stage.Add<CuratorTestsFixture::DifferentiableCurator<id>>();
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
        std::vector<CuratorTestsFixture::CuratorCheckpoint>& checkpoints,
        Reliquary& reliquary,
        std::list<bool>& output)
    {
        if (checkpoints.size() < id + 1)
            return;

        const auto& curator = reliquary.Find<CuratorTestsFixture::DifferentiableCurator<id>>();

        output.push_back(checkpoints[id].curator == &curator);
    }

    static void Do(
        std::vector<Curator*>& curators,
        Reliquary& reliquary,
        std::list<bool>& output)
    {
        const auto curator = reliquary.Find<CuratorTestsFixture::DifferentiableCurator<id>>();

        const auto position = curators[id];
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
        ::Chroma::IterateRange<size_t, StagePerCuratorIterator, 99, 0>(pipeline);

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline);
        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 99, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();

        WHEN("working reliquary")
        {
            reliquary->Work();

            THEN("executes all in order")
            {
                REQUIRE(checkpoints.size() == 100);

                std::list<bool> output;

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (checkpoints, *reliquary, output);

                REQUIRE(output.size() == 100);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
                output.clear();
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
                REQUIRE_THROWS_AS(reliquaryOrigin.Actualize(), NotRegistered);
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
        ::Chroma::IterateRangeBackward<size_t, StagePerCuratorIterator, 99, 0>(initializationPipeline);
        auto workPipeline = Pipeline();
        ::Chroma::IterateRange<size_t, StagePerCuratorIterator, 99, 0>(workPipeline);

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
                (checkpoints, *reliquary, output);
        }

        WHEN("working reliquary")
        {
            reliquary->Work();

            THEN("executes all in order")
            {
                REQUIRE(checkpoints.size() == 100);

                std::list<bool> output;

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (checkpoints, *reliquary, output);

                REQUIRE(output.size() == 100);
                REQUIRE(std::all_of(
                    output.begin(),
                    output.end(),
                    [](const bool& entry) { return entry; }));
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
                REQUIRE_THROWS_AS(reliquaryOrigin.Actualize(), NotRegistered);
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

SCENARIO_METHOD(CuratorTestsFixture, "curator aborts pipeline", "[curator][pipeline]")
{
    GIVEN("reliquary with pipeline with one hundred stages with one curator in each")
    {
        std::vector<CuratorCheckpoint> checkpoints;
        std::vector<Curator*> initializationCheckpoints;
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 99, 0>(checkpoints, initializationCheckpoints);

        auto pipeline = Pipeline();
        ::Chroma::IterateRange<size_t, StagePerCuratorIterator, 99, 0>(pipeline);

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline);
        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 99, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();

        WHEN("working reliquary when middle curator will abort")
        {
            auto& middleCurator = reliquary->Find<DifferentiableCurator<49>>();
            middleCurator.shouldAbort = true;

            reliquary->Work();

            THEN("executed up until 51st")
            {
                REQUIRE(checkpoints.size() == 50);

                std::list<bool> output;

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    99,
                    0
                >
                    (checkpoints, *reliquary, output);

                auto outputMiddle = std::next(output.begin(), 50);

                REQUIRE(output.size() == 50);
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

    GIVEN("nine curators in three stages")
    {
        std::vector<CuratorCheckpoint> checkpoints;
        std::vector<Curator*> initializationCheckpoints;
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 8, 0>(checkpoints, initializationCheckpoints);

        auto pipeline = Pipeline();
        auto stage1 = pipeline.emplace_back();
        auto stage2 = pipeline.emplace_back();
        auto stage3 = pipeline.emplace_back();
        ::Chroma::IterateRange<size_t, StageIterator, 2, 0>(stage1);
        ::Chroma::IterateRange<size_t, StageIterator, 5, 3>(stage2);
        ::Chroma::IterateRange<size_t, StageIterator, 8, 6>(stage3);

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline);
        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 8, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();

        WHEN("working reliquary when middle stage will abort")
        {
            auto& middleCurator = reliquary->Find<DifferentiableCurator<4>>();
            middleCurator.shouldAbort = true;

            reliquary->Work();

            THEN("executed all up to and including aborting curator")
            {
                REQUIRE(checkpoints.size() == 5);

                std::list<bool> output;

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointVerification,
                    8,
                    0
                >
                    (checkpoints, *reliquary, output);

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

SCENARIO_METHOD(CuratorTestsFixture, "curator serialization", "[curator][serialization]")
{
    GIVEN("saved reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Type<BasicCurator>()
            .Actualize();

        auto& savedCurator = savedReliquary->Find<BasicCurator>();
        savedCurator.value = dataGeneration.Random<int>();

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
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
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
                .Type<OtherBasicCurator>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");;
                inputArchive(*loadedReliquary);
            }

            auto& loadedCurator = loadedReliquary->Find<OtherBasicCurator>();

            THEN("value is loaded")
            {
                REQUIRE(loadedCurator.value == savedCurator.value);
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

    std::vector<Type> Scribe<CuratorTestsFixture::OtherBasicCurator, BinaryArchive>::InputTypes(
        const ArchiveT&)
    {
        return { "BasicCurator" };
    }
}