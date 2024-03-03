#include <catch.hpp>

#include "CuratorTests.h"

#include <Chroma/Iterate.h>

CuratorTestsFixture::CuratorTestsFixture()
{
    auto group = typeRegistration.CreateGroup();
    group->RegisterCurator<BasicCurator>();
}

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

CuratorTestsFixture::BasicCurator::BasicCurator(Reliquary& owner) : Curator(owner)
{}

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

CuratorTestsFixture::OtherBasicCurator::OtherBasicCurator(Reliquary& owner) : Curator(owner)
{}

namespace Arca
{
    template<>
    struct CuratorTraits<CuratorTestsFixture::BasicCurator>
    {
        static const TypeHandle typeHandle;
    };

    const TypeHandle CuratorTraits<CuratorTestsFixture::BasicCurator>::typeHandle = "BasicCurator";

    template<>
    struct CuratorTraits<CuratorTestsFixture::OtherBasicCurator>
    {
        static const TypeHandle typeHandle;
    };

    const TypeHandle CuratorTraits<CuratorTestsFixture::OtherBasicCurator>::typeHandle = "OtherBasicCurator";
}

SCENARIO_METHOD(CuratorTestsFixture, "curator", "[curator]")
{
    GIVEN("reliquary registered and initialized with curator without pipeline")
    {
        std::vector<CuratorState> states;

        auto reliquary = ReliquaryOrigin()
            .Curator<BasicCurator>()
            .Actualize();

        auto curator = reliquary.FindCurator<BasicCurator>();
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

        WHEN("finding curator")
        {
            THEN("found is not null")
            {
                REQUIRE(curator != nullptr);
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
                REQUIRE(&curator->OwnerFromOutside() == &reliquary);
            }

            THEN("owner is reliquary const")
            {
                const auto constCurator = curator;
                REQUIRE(&constCurator->OwnerFromOutside() == &reliquary);
            }
        }

        WHEN("working reliquary")
        {
            reliquary.Work();

            THEN("curator ran through states in order")
            {
                REQUIRE(states.size() == 3);
                REQUIRE(states[0] == CuratorState::Started);
                REQUIRE(states[1] == CuratorState::Worked);
                REQUIRE(states[2] == CuratorState::Stopped);
            }
        }

        WHEN("curator should not be started")
        {
            curator->shouldStart = false;

            reliquary.Work();

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
    static void Do(CuratorPipeline& pipeline)
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
        reliquaryOrigin.Curator<CuratorTestsFixture::DifferentiableCurator<id>>();
    }
};

template<size_t id>
struct SetupDifferentiableCurator
{
    static void Do(std::vector<CuratorTestsFixture::CuratorCheckpoint>& checkpoints)
    {
        CuratorTestsFixture::DifferentiableCurator<id>::checkpoints = &checkpoints;
    }
};

template<size_t id>
struct RequireDifferentiableCuratorCheckpointCadence
{
    static void Do(
        std::vector<CuratorTestsFixture::CuratorCheckpoint>::iterator beginPosition,
        Reliquary& reliquary,
        std::list<bool>& output)
    {
        auto curator = reliquary.FindCurator<CuratorTestsFixture::DifferentiableCurator<id>>();

        auto position = beginPosition + id;
        output.push_back(position->curator == curator);
    }
};

SCENARIO_METHOD(CuratorTestsFixture, "curator pipeline", "[curator][pipeline]")
{
    GIVEN("reliquary with pipeline with one hundred stages with one curator in each")
    {
        std::vector<CuratorCheckpoint> checkpoints;
        ::Chroma::IterateRange<size_t, SetupDifferentiableCurator, 99, 0>(checkpoints);

        auto pipeline = CuratorPipeline();
        ::Chroma::IterateRange<size_t, PipelineIterator, 99, 0>(pipeline);

        auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline);
        ::Chroma::IterateRange<size_t, ReliquaryOriginIterator, 99, 0>(reliquaryOrigin);

        auto reliquary = reliquaryOrigin.Actualize();

        WHEN("working reliquary")
        {
            reliquary.Work();

            THEN("executes all in order")
            {
                REQUIRE(checkpoints.size() == 300);

                std::list<bool> output;
                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointCadence,
                    99,
                    0
                >
                    (checkpoints.begin(), reliquary, output);

                REQUIRE(output.size() == 100);
                for (auto& currentOutput : output)
                    REQUIRE(currentOutput);
                output.clear();

                for (auto itr = checkpoints.begin(); itr != checkpoints.begin() + 100; ++itr)
                    REQUIRE(itr->state == CuratorState::Started);

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointCadence,
                    99,
                    0
                >
                    (checkpoints.begin() + 100, reliquary, output);

                REQUIRE(output.size() == 100);
                for (auto& currentOutput : output)
                    REQUIRE(currentOutput);
                output.clear();

                for (auto itr = checkpoints.begin() + 100; itr != checkpoints.begin() + 200; ++itr)
                    REQUIRE(itr->state == CuratorState::Worked);

                ::Chroma::IterateRange<
                    size_t,
                    RequireDifferentiableCuratorCheckpointCadence,
                    99,
                    0
                >
                    (checkpoints.begin() + 200, reliquary, output);

                REQUIRE(output.size() == 100);
                for (auto& currentOutput : output)
                    REQUIRE(currentOutput);
                output.clear();

                for (auto itr = checkpoints.begin() + 200; itr != checkpoints.begin() + 300; ++itr)
                    REQUIRE(itr->state == CuratorState::Stopped);
            }
        }
    }

    GIVEN("reliquary origin with pipeline that contains nonexistent curator")
    {
        auto pipeline = CuratorPipeline();
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
                    InvalidCuratorPipeline,
                    ::Catch::Matchers::Message("Curator (" + CuratorTraits<BasicCurator>::typeHandle + ") was not found.")
                );
            }
        }
    }

    GIVEN("reliquary origin with pipeline that contains curator multiple times")
    {
        auto pipeline = CuratorPipeline();
        pipeline.emplace_back();
        pipeline.back().Add<BasicCurator>();
        pipeline.back().Add<BasicCurator>();

        const auto reliquaryOrigin = ReliquaryOrigin()
            .CuratorPipeline(pipeline)
            .Curator<BasicCurator>();

        WHEN("actualized")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    InvalidCuratorPipeline,
                    ::Catch::Matchers::Message("Curator (" + CuratorTraits<BasicCurator>::typeHandle + ") " +
                        "was already in the pipeline.")
                );
            }
        }
    }

    GIVEN("reliquary origin with pipeline that contains only empty stage")
    {
        auto pipeline = CuratorPipeline();
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
            .Curator<BasicCurator>()
            .Actualize();

        auto savedCurator = savedReliquary.FindCurator<BasicCurator>();
        savedCurator->value = dataGeneration.Random<int>();

        {
            auto outputArchive = CreateRegistered<::Inscription::OutputBinaryArchive>();
            outputArchive(savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Curator<BasicCurator>()
                .Actualize();

            {
                auto inputArchive = CreateRegistered<::Inscription::InputBinaryArchive>();
                inputArchive(loadedReliquary);
            }

            auto loadedCurator = loadedReliquary.FindCurator<BasicCurator>();

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
                auto inputArchive = CreateRegistered<::Inscription::InputBinaryArchive>();
                inputArchive(loadedReliquary);
            }

            auto loadedCurator = loadedReliquary.FindCurator<BasicCurator>();

            THEN("loaded curator is null")
            {
                REQUIRE(loadedCurator == nullptr);
            }
        }

        WHEN("loading reliquary with different curator with same input type handle")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Curator<OtherBasicCurator>()
                .Actualize();

            {
                auto inputArchive = CreateRegistered<::Inscription::InputBinaryArchive>();
                inputArchive(loadedReliquary);
            }

            auto loadedCurator = loadedReliquary.FindCurator<OtherBasicCurator>();

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