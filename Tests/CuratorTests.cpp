#include <catch.hpp>

#include "CuratorTests.h"

CuratorTestsFixture::CuratorTestsFixture()
{
    auto group = typeRegistration.CreateGroup();
    group->RegisterCurator<BasicCurator>();
}

CuratorTestsFixture::BasicCurator::BasicCurator(Reliquary& owner) : Curator(owner)
{}

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
    GIVEN("reliquary registered and initialized with curator")
    {
        auto reliquary = ReliquaryOrigin()
            .Curator<BasicCurator>()
            .Actualize();

        WHEN("finding curator")
        {
            auto curator = reliquary.FindCurator<BasicCurator>();

            THEN("found is not null")
            {
                REQUIRE(curator != nullptr);
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