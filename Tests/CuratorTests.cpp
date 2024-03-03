#include <catch.hpp>

#include "CuratorTests.h"

CuratorTestsFixture::CuratorTestsFixture()
{
    auto group = typeRegistration.CreateGroup();
    group->RegisterCurator<BasicCurator>();
}

CuratorTestsFixture::BasicCurator::BasicCurator(Reliquary& owner) : Curator(owner)
{}

namespace Arca
{
    template<>
    struct CuratorTraits<CuratorTestsFixture::BasicCurator>
    {
        static const TypeHandle typeHandle;
    };

    const TypeHandle CuratorTraits<CuratorTestsFixture::BasicCurator>::typeHandle = "BasicCurator";
}

SCENARIO_METHOD(CuratorTestsFixture, "Curator")
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

    GIVEN("type registration pushed to reliquary")
    {
        ReliquaryOrigin origin;
        typeRegistration.PushAllTo(origin);
        auto reliquary = origin.Actualize();

        /*
        WHEN("saving and loading reliquary")
        {
            auto curator = reliquary.FindCurator<BasicCurator>();
            curator->value = dataGeneration.Random<int>();

            {
                auto outputArchive = CreateRegistered<::Inscription::OutputBinaryArchive>();
                outputArchive(reliquary);
            }

            ReliquaryOrigin loadedOrigin;
            typeRegistration.PushAllTo(loadedOrigin);
            auto loadedReliquary = loadedOrigin.Actualize();

            {
                auto inputArchive = CreateRegistered<::Inscription::InputBinaryArchive>();
                inputArchive(loadedReliquary);
            }

            auto found = loadedReliquary.FindCurator<BasicCurator>();

            THEN("loaded curator is not null")
            {
                REQUIRE(found != nullptr);
            }

            THEN("value is loaded")
            {
                REQUIRE(found->value == curator->value);
            }
        }
        */
    }
}

namespace Inscription
{
    void Scribe<CuratorTestsFixture::BasicCurator, BinaryArchive>::ScrivenImplementation(
        ObjectT& object, ArchiveT& archive)
    {
        archive(object.value);
    }

    TypeHandle Scribe<CuratorTestsFixture::BasicCurator, BinaryArchive>::PrincipleTypeHandle(const ArchiveT& archive)
    {
        return "BasicCurator";
    }
}