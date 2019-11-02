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

ReliquaryTestsFixture::BasicRelic::BasicRelic(const ::Inscription::BinaryTableData<BasicRelic>& data) :
    myValue(data.myValue)
{}

ReliquaryTestsFixture::OtherRelic::OtherRelic(int myValue) : myValue(myValue)
{}

ReliquaryTestsFixture::OtherRelic::OtherRelic(const ::Inscription::BinaryTableData<OtherRelic>& data) :
    myValue(data.myValue)
{}

ReliquaryTestsFixture::BasicTypedVessel::BasicTypedVessel(VesselID id, Reliquary& owner) :
    TypedVessel(id, owner)
{
    Setup();
}

ReliquaryTestsFixture::BasicTypedVessel::BasicTypedVessel(const ::Inscription::BinaryTableData<BasicTypedVessel>& data) :
    TypedVessel(data.base)
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

ReliquaryTestsFixture::StaticVessel::StaticVessel(const ::Inscription::BinaryTableData<StaticVessel>& data) :
    TypedVessel(data.base)
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

SCENARIO_METHOD(ReliquaryTestsFixture, "reliquary serialization", "[reliquary]")
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

    }
}