#include <catch.hpp>
#include <utility>

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

ReliquaryTestsFixture::StaticVessel::StaticVessel(VesselID id, Reliquary& owner) : TypedVessel(id, owner)
{}

ReliquaryTestsFixture::StaticVessel::StaticVessel(const ::Inscription::BinaryTableData<StaticVessel>& data) :
    TypedVessel(data.base)
{}

ReliquaryTestsFixture::BasicCurator::BasicCurator(Reliquary& owner) : Curator(owner)
{}

namespace Arca
{
    const TypeHandle RelicTraits<::ReliquaryTestsFixture::BasicRelic>::typeHandle =
        "ReliquaryTestsBasicRelic";

    const TypeHandle VesselTraits<::ReliquaryTestsFixture::StaticVessel>::typeHandle =
        "ReliquaryTestsStaticVessel";

    const TypeHandle CuratorTraits<::ReliquaryTestsFixture::BasicCurator>::typeHandle =
        "ReliquaryTestsBasicCurator";
}

SCENARIO_METHOD(ReliquaryTestsFixture, "Reliquary errors")
{
    GIVEN("empty reliquary")
    {
        auto reliquary = ReliquaryOrigin().Actualize();

        WHEN("checking vessel count")
        {
            THEN("has vessel count of zero")
            {
                REQUIRE(reliquary.VesselCount() == 0);
            }
        }

        WHEN("creating relic on vessel")
        {
            auto vessel = reliquary.CreateVessel();

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(vessel.CreateRelic<BasicRelic>(), NotRegistered);
            }
        }

        WHEN("retrieving static vessel")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.StaticVessel<StaticVessel>(), NotRegistered);
            }
        }

        WHEN("finding curator")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.FindCurator<BasicCurator>(), NotRegistered());
            }
        }

        WHEN("raising signal")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.RaiseSignal(BasicSignal()), NotRegistered());
            }
        }
    }

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