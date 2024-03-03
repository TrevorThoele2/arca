#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "ReliquaryTests.h"

ReliquaryTestsFixture::ReliquaryTestsFixture()
{
    auto typeGroup = typeRegistration.CreateGroup();
    typeGroup->RegisterRelic<BasicRelic>();
}

ReliquaryTestsFixture::BasicRelic::BasicRelic(std::string myValue) : myValue(std::move(myValue))
{}

ReliquaryTestsFixture::OtherRelic::OtherRelic(int myValue) : myValue(myValue)
{}

ReliquaryTestsFixture::BasicTypedVessel::BasicTypedVessel(VesselID id, Reliquary& owner) :
    TypedVessel(id, owner)
{
    Setup();
}

void ReliquaryTestsFixture::BasicTypedVessel::Setup()
{
    ExtractRelics(RelicTuple(basicRelic));
}

ReliquaryTestsFixture::StaticVessel::StaticVessel(VesselID id, Reliquary& owner) : TypedVessel(id, owner)
{
    Setup();
}

void ReliquaryTestsFixture::StaticVessel::Setup()
{
    ExtractRelics(RelicTuple(basicRelic));
}

ReliquaryTestsFixture::BasicCurator::BasicCurator(Reliquary& owner) : Curator(owner)
{}

namespace Arca
{
    const TypeHandle RelicTraits<::ReliquaryTestsFixture::BasicRelic>::typeHandle =
        "ReliquaryTestsBasicRelic";

    const TypeHandle RelicTraits<::ReliquaryTestsFixture::OtherBasicRelic>::typeHandle =
        "ReliquaryTestsOtherBasicRelic";

    const TypeHandle RelicTraits<::ReliquaryTestsFixture::OtherRelic>::typeHandle =
        "ReliquaryTestsOtherRelic";

    const TypeHandle VesselTraits<::ReliquaryTestsFixture::BasicTypedVessel>::typeHandle =
        "ReliquaryTestsBasicTypedVessel";

    const TypeHandle VesselTraits<::ReliquaryTestsFixture::StaticVessel>::typeHandle =
        "ReliquaryTestsStaticVessel";

    const TypeHandle CuratorTraits<::ReliquaryTestsFixture::BasicCurator>::typeHandle =
        "ReliquaryTestsBasicCurator";
}

SCENARIO_METHOD(ReliquaryTestsFixture, "default reliquary", "[reliquary]")
{
    GIVEN("default reliquary")
    {
        auto reliquary = ReliquaryOrigin().Actualize();

        WHEN("checking vessel count")
        {
            THEN("has vessel count of zero")
            {
                REQUIRE(reliquary.VesselCount() == 0);
            }
        }

        WHEN("finding vessel")
        {
            auto found = reliquary.FindVessel(1);

            THEN("is empty")
            {
                REQUIRE(!found.has_value());
            }
        }

        WHEN("finding relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.FindRelic<BasicRelic>(1),
                    NotRegistered,
                    Catch::Matchers::Message("The relic (ReliquaryTestsBasicRelic) was not registered.")
                );
            }
        }

        WHEN("creating vessel")
        {
            reliquary.CreateVessel();

            THEN("reliquary has vessel count of one")
            {
                REQUIRE(reliquary.VesselCount() == 1);
            }
        }

        WHEN("creating typed vessel with unregistered relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.CreateVessel<BasicTypedVessel>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The relic ("s + RelicTraits<BasicRelic>::typeHandle + ") was not registered.")
                );
            }
        }

        WHEN("retrieving static vessel")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.StaticVessel<StaticVessel>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The static vessel ("s + VesselTraits<StaticVessel>::typeHandle + ") was not registered.")
                );
            }
        }

        WHEN("finding curator")
        {
            THEN("returns nullptr")
            {
                REQUIRE(reliquary.FindCurator<BasicCurator>() == nullptr);
            }
        }

        WHEN("finding curator const")
        {
            const auto& constReliquary = reliquary;

            THEN("returns nullptr")
            {
                REQUIRE(constReliquary.FindCurator<BasicCurator>() == nullptr);
            }
        }

        WHEN("raising signal")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.RaiseSignal(BasicSignal()),
                    NotRegistered,
                    Catch::Matchers::Message("The signal ("s + typeid(BasicSignal).name() + ") was not registered.")
                );
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "registered reliquary with every type", "[reliquary]")
{
    GIVEN("all registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<BasicRelic>()
            .StaticVessel<StaticVessel>()
            .Curator<BasicCurator>()
            .CuratorLayout(CuratorLayout())
            .Signal<BasicSignal>()
            .Actualize();

        WHEN("checking vessel count")
        {
            THEN("has count of one for static relic")
            {
                REQUIRE(reliquary.VesselCount() == 1);
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "reliquary serialization", "[reliquary][serialization]")
{
    GIVEN("saved empty reliquary with every type registered")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Relic<BasicRelic>()
            .StaticVessel<StaticVessel>()
            .Curator<BasicCurator>()
            .CuratorLayout(CuratorLayout())
            .Signal<BasicSignal>()
            .Actualize();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Relic<BasicRelic>()
                .StaticVessel<StaticVessel>()
                .Curator<BasicCurator>()
                .CuratorLayout(CuratorLayout())
                .Signal<BasicSignal>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(loadedReliquary);
            }

            THEN("has only static vessel")
            {
                REQUIRE(loadedReliquary.VesselCount() == 1);
            }
        }
    }

    GIVEN("saved reliquary with dynamic vessel")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Relic<BasicRelic>()
            .Actualize();

        auto savedVessel = savedReliquary.CreateVessel();
        auto savedRelic = savedVessel.CreateRelic<BasicRelic>();
        savedRelic->myValue = dataGeneration.Random<std::string>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Relic<BasicRelic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(loadedReliquary);
            }

            auto loadedVessel = loadedReliquary.FindVessel(savedVessel.ID());
            auto relicFromVessel = loadedVessel->FindRelic<BasicRelic>();

            THEN("has vessel")
            {
                REQUIRE(loadedReliquary.VesselCount() == 1);
                REQUIRE(loadedVessel.has_value());
            }

            THEN("vessel has relic")
            {
                auto relicFromReliquary = loadedReliquary.FindRelic<BasicRelic>(loadedVessel->ID());
                REQUIRE(relicFromReliquary != nullptr);
                REQUIRE(relicFromVessel != nullptr);
                REQUIRE(loadedVessel->HasRelic<BasicRelic>());
            }

            THEN("relic has saved value")
            {
                REQUIRE(relicFromVessel->myValue == savedRelic->myValue);
            }

            THEN("vessel is dynamic")
            {
                REQUIRE(loadedVessel->Dynamism() == VesselDynamism::Dynamic);
            }

            THEN("vessel owner is loaded reliquary")
            {
                REQUIRE(&loadedVessel->Owner() == &loadedReliquary);
            }

            THEN("vessel id is saved id")
            {
                REQUIRE(loadedVessel->ID() == savedVessel.ID());
            }
        }

        WHEN("loading reliquary without registering relic type")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(loadedReliquary);
            }

            auto loadedVessel = loadedReliquary.FindVessel(savedVessel.ID());

            THEN("has vessel")
            {
                REQUIRE(loadedReliquary.VesselCount() == 1);
                REQUIRE(loadedVessel.has_value());
            }

            THEN("vessel does not have relic")
            {
                REQUIRE_THROWS_AS(loadedVessel->FindRelic<BasicRelic>(), NotRegistered);
                REQUIRE_THROWS_AS(loadedReliquary.FindRelic<BasicRelic>(loadedVessel->ID()), NotRegistered);
            }

            THEN("vessel is dynamic")
            {
                REQUIRE(loadedVessel->Dynamism() == VesselDynamism::Dynamic);
            }

            THEN("vessel owner is loaded reliquary")
            {
                REQUIRE(&loadedVessel->Owner() == &loadedReliquary);
            }

            THEN("vessel id is saved id")
            {
                REQUIRE(loadedVessel->ID() == savedVessel.ID());
            }
        }

        WHEN("loading reliquary with different relic type with same input type handle")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Relic<OtherBasicRelic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(loadedReliquary);
            }

            auto loadedVessel = loadedReliquary.FindVessel(savedVessel.ID());
            auto relicFromVessel = loadedVessel->FindRelic<OtherBasicRelic>();

            THEN("has vessel")
            {
                REQUIRE(loadedReliquary.VesselCount() == 1);
                REQUIRE(loadedVessel.has_value());
            }

            THEN("vessel has relic")
            {
                auto relicFromReliquary = loadedReliquary.FindRelic<OtherBasicRelic>(loadedVessel->ID());
                REQUIRE(relicFromReliquary != nullptr);
                REQUIRE(relicFromVessel != nullptr);
                REQUIRE(loadedVessel->HasRelic<OtherBasicRelic>());
            }

            THEN("relic has saved value")
            {
                REQUIRE(relicFromVessel->myValue == savedRelic->myValue);
            }

            THEN("vessel is dynamic")
            {
                REQUIRE(loadedVessel->Dynamism() == VesselDynamism::Dynamic);
            }

            THEN("vessel owner is loaded reliquary")
            {
                REQUIRE(&loadedVessel->Owner() == &loadedReliquary);
            }

            THEN("vessel id is saved id")
            {
                REQUIRE(loadedVessel->ID() == savedVessel.ID());
            }
        }
    }
}