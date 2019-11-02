#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "VesselTests.h"

#include <Chroma/StringUtility.h>

VesselTestsFixture::BasicRelic::BasicRelic(std::string myValue) : myValue(std::move(myValue))
{}

VesselTestsFixture::OtherRelic::OtherRelic(int myValue) : myValue(myValue)
{}

VesselTestsFixture::OtherRelic::OtherRelic(const ::Inscription::BinaryTableData<OtherRelic>& data) :
    myValue(data.myValue)
{}

VesselTestsFixture::BasicTypedVessel::BasicTypedVessel(VesselID id, Reliquary& owner) :
    TypedVessel(id, owner)
{
    Setup();
}

VesselTestsFixture::BasicTypedVessel::BasicTypedVessel(const ::Inscription::BinaryTableData<BasicTypedVessel>& data) :
    TypedVessel(data.base)
{
    Setup();
}

void VesselTestsFixture::BasicTypedVessel::Setup()
{
    ExtractRelics(RelicTuple(basicRelic));
}

VesselTestsFixture::StaticVessel::StaticVessel(VesselID id, Reliquary& owner) : TypedVessel(id, owner)
{
    Setup();
}

VesselTestsFixture::StaticVessel::StaticVessel(const ::Inscription::BinaryTableData<StaticVessel>& data) :
    TypedVessel(data.base)
{
    Setup();
}

void VesselTestsFixture::StaticVessel::Setup()
{
    ExtractRelics(RelicTuple(basicRelic));
}

namespace Arca
{
    const TypeHandle RelicTraits<VesselTestsFixture::BasicRelic>::typeHandle =
        "VesselTestsBasicRelic";

    const TypeHandle RelicTraits<VesselTestsFixture::OtherRelic>::typeHandle =
        "ReliquaryTestsOtherRelic";

    const TypeHandle VesselTraits<VesselTestsFixture::BasicTypedVessel>::typeHandle =
        "ReliquaryTestsBasicTypedVessel";

    const TypeHandle VesselTraits<VesselTestsFixture::StaticVessel>::typeHandle =
        "ReliquaryTestsStaticVessel";
}

SCENARIO_METHOD(VesselTestsFixture, "vessel")
{
    GIVEN("all types registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<BasicRelic>()
            .Relic<OtherRelic>()
            .StaticVessel<StaticVessel>()
            .Actualize();

        WHEN("creating dynamic vessel")
        {
            auto preCreateVesselCount = reliquary.VesselCount();
            auto vessel = reliquary.CreateVessel();

            THEN("vessel is dynamic")
            {
                REQUIRE(vessel.Dynamism() == VesselDynamism::Dynamic);
            }

            THEN("reliquary vessel count increments by one")
            {
                REQUIRE(reliquary.VesselCount() == (preCreateVesselCount + 1));
            }

            WHEN("creating relic")
            {
                auto relic = vessel.CreateRelic<BasicRelic>();

                THEN("relic is returned")
                {
                    REQUIRE(relic != nullptr);
                }

                THEN("vessel has relic")
                {
                    REQUIRE(vessel.FindRelic<BasicRelic>() != nullptr);
                    REQUIRE(vessel.HasRelic<BasicRelic>());
                }

                WHEN("destroying relic")
                {
                    vessel.DestroyRelic<BasicRelic>();

                    THEN("vessel does not have relic")
                    {
                        REQUIRE(vessel.FindRelic<BasicRelic>() == nullptr);
                        REQUIRE(!vessel.HasRelic<BasicRelic>());
                    }
                }
            }
        }

        WHEN("creating fixed vessel with valid structure")
        {
            auto preCreateVesselCount = reliquary.VesselCount();
            auto vessel = reliquary.CreateVessel(VesselStructure{ RelicTraits<BasicRelic>::typeHandle });

            THEN("structure has been satisfied")
            {
                auto relicFromReliquary = reliquary.FindRelic<BasicRelic>(vessel.ID());
                REQUIRE(relicFromReliquary != nullptr);

                auto relicFromVessel = vessel.FindRelic<BasicRelic>();
                REQUIRE(relicFromVessel != nullptr);
                REQUIRE(vessel.HasRelic<BasicRelic>());
            }

            THEN("relics cannot be created later")
            {
                REQUIRE_THROWS_AS(vessel.CreateRelic<OtherRelic>(), CannotCreateRelic);
            }

            THEN("relics cannot be destroyed later")
            {
                REQUIRE_THROWS_AS(vessel.DestroyRelic<OtherRelic>(), CannotDestroyRelic);
            }

            THEN("vessel is fixed")
            {
                REQUIRE(vessel.Dynamism() == VesselDynamism::Fixed);
            }

            THEN("reliquary vessel count increments by one")
            {
                REQUIRE(reliquary.VesselCount() == (preCreateVesselCount + 1));
            }

            WHEN("destroying vessel")
            {
                auto preDestroyVesselCount = reliquary.VesselCount();

                auto id = vessel.ID();
                reliquary.DestroyVessel(vessel);

                THEN("finding vessel returns empty")
                {
                    auto found = reliquary.FindVessel(id);
                    REQUIRE(!found.has_value());
                }

                THEN("destroying again does not throw")
                {
                    REQUIRE_NOTHROW(reliquary.DestroyVessel(vessel));
                }

                THEN("reliquary vessel count decrements by one")
                {
                    REQUIRE(reliquary.VesselCount() == (preDestroyVesselCount - 1));
                }
            }
        }

        WHEN("retrieving static vessel")
        {
            const auto staticVessel = reliquary.StaticVessel<StaticVessel>();

            THEN("structure has been satisfied")
            {
                auto relicFromReliquary = reliquary.FindRelic<BasicRelic>(staticVessel.ID());
                REQUIRE(relicFromReliquary != nullptr);

                auto relicFromVessel = staticVessel.basicRelic;
                REQUIRE(relicFromVessel != nullptr);
            }

            THEN("owner is correct")
            {
                REQUIRE(&staticVessel.Owner() == &reliquary);
            }

            THEN("cannot destroy vessel")
            {
                auto preDestroyVesselCount = reliquary.VesselCount();

                reliquary.DestroyVessel(staticVessel);

                auto foundAgain = reliquary.StaticVessel<StaticVessel>();

                REQUIRE(foundAgain.ID() == staticVessel.ID());
                REQUIRE(foundAgain.basicRelic == staticVessel.basicRelic);
                REQUIRE(preDestroyVesselCount == reliquary.VesselCount());
            }
        }

        WHEN("retrieving static vessel by id")
        {
            const auto idProvider = reliquary.StaticVessel<StaticVessel>();
            const auto staticVessel = reliquary.FindVessel(idProvider.ID());

            THEN("returns value")
            {
                REQUIRE(staticVessel.has_value());
            }

            THEN("structure has been satisfied")
            {
                auto relicFromReliquary = reliquary.FindRelic<BasicRelic>(staticVessel->ID());
                REQUIRE(relicFromReliquary != nullptr);

                auto relicFromVessel = staticVessel->FindRelic<BasicRelic>();
                REQUIRE(relicFromVessel != nullptr);
            }

            THEN("owner is correct")
            {
                REQUIRE(&staticVessel->Owner() == &reliquary);
            }

            THEN("cannot destroy vessel")
            {
                auto preDestroyVesselCount = reliquary.VesselCount();

                reliquary.DestroyVessel(*staticVessel);

                auto foundAgain = reliquary.StaticVessel<StaticVessel>();

                REQUIRE(foundAgain.ID() == staticVessel->ID());
                REQUIRE(foundAgain.basicRelic == staticVessel->FindRelic<BasicRelic>());
                REQUIRE(preDestroyVesselCount == reliquary.VesselCount());
            }
        }

        WHEN("retrieving static vessel by typed finder")
        {
            const auto idProvider = reliquary.StaticVessel<StaticVessel>();
            const auto staticVessel = reliquary.FindVessel<StaticVessel>(idProvider.ID());

            THEN("returns value")
            {
                REQUIRE(staticVessel.has_value());
            }

            THEN("structure has been satisfied")
            {
                auto relicFromReliquary = reliquary.FindRelic<BasicRelic>(staticVessel->ID());
                REQUIRE(relicFromReliquary != nullptr);

                auto relicFromVessel = staticVessel->basicRelic;
                REQUIRE(relicFromVessel != nullptr);
            }

            THEN("owner is correct")
            {
                REQUIRE(&staticVessel->Owner() == &reliquary);
            }

            THEN("cannot destroy vessel")
            {
                auto preDestroyVesselCount = reliquary.VesselCount();

                reliquary.DestroyVessel(*staticVessel);

                auto foundAgain = reliquary.StaticVessel<StaticVessel>();

                REQUIRE(foundAgain.ID() == staticVessel->ID());
                REQUIRE(foundAgain.basicRelic == staticVessel->basicRelic);
                REQUIRE(preDestroyVesselCount == reliquary.VesselCount());
            }
        }

        WHEN("retrieving unregistered static vessel")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.StaticVessel<BasicTypedVessel>(),
                    NotRegistered,
                    ::Catch::Matchers::Message("The static vessel (ReliquaryTestsBasicTypedVessel) was not registered.")
                );
            }
        }
    }
}

SCENARIO_METHOD(VesselTestsFixture, "vessel parenting", "[vessel][parenting]")
{
    GIVEN("parent and child created")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<BasicRelic>()
            .Actualize();

        auto parent = reliquary.CreateVessel();
        parent.CreateRelic<BasicRelic>();
        auto child = reliquary.CreateVessel();
        child.CreateRelic<BasicRelic>();

        WHEN("parenting child to parent")
        {
            reliquary.ParentVessel(parent.ID(), child.ID());

            THEN("destroying parent also destroys child")
            {
                reliquary.DestroyVessel(parent);

                REQUIRE(reliquary.VesselCount() == 0);
                REQUIRE(reliquary.FindRelic<BasicRelic>(child.ID()) == nullptr);
            }

            WHEN("destroying child")
            {
                reliquary.DestroyVessel(child);

                THEN("does not destroy parent")
                {
                    REQUIRE(reliquary.VesselCount() == 1);
                    REQUIRE(reliquary.FindRelic<BasicRelic>(parent.ID()) != nullptr);
                }

                THEN("destroying parent works")
                {
                    reliquary.DestroyVessel(parent);

                    REQUIRE(reliquary.VesselCount() == 0);
                    REQUIRE(reliquary.FindRelic<BasicRelic>(parent.ID()) == nullptr);
                }
            }

            WHEN("parenting child to parent again")
            {
                THEN("throws error")
                {
                    REQUIRE_THROWS_MATCHES
                    (
                        reliquary.ParentVessel(parent.ID(), child.ID()),
                        VesselAlreadyParented,
                        ::Catch::Matchers::Message(
                            "The vessel with id ("s + ::Chroma::ToString(child.ID()) + ") has already been parented.")
                    );
                }
            }
        }

        WHEN("parenting child to nonexistent vessel")
        {
            THEN("throws error")
            {
                auto invalidId = -1;

                REQUIRE_THROWS_MATCHES
                (
                    reliquary.ParentVessel(invalidId, child.ID()),
                    CannotFindVessel,
                    ::Catch::Matchers::Message(
                        "The vessel with id ("s + ::Chroma::ToString(invalidId) + ") cannot be found.")
                );
            }
        }

        WHEN("parenting nonexistent vessel to parent")
        {
            THEN("throws error")
            {
                auto invalidId = -1;

                REQUIRE_THROWS_MATCHES
                (
                    reliquary.ParentVessel(parent.ID(), invalidId),
                    CannotFindVessel,
                    ::Catch::Matchers::Message(
                        "The vessel with id ("s + ::Chroma::ToString(invalidId) + ") cannot be found.")
                );
            }
        }

        WHEN("parenting vessel to itself")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.ParentVessel(parent.ID(), parent.ID()),
                    CannotParentVesselToSelf,
                    ::Catch::Matchers::Message(
                        "The vessel with id (" + ::Chroma::ToString(parent.ID()) + ") " + 
                        "was attempted to be parented to itself.")
                );
            }
        }
    }
}