#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryOriginRegistrationTests.h"

SCENARIO_METHOD(ReliquaryOriginRegistrationTestsFixture, "registering types multiple times", "[reliquaryorigin]")
{
    GIVEN("all types registered to origin")
    {
        const auto relicStructureName = dataGeneration.Random<std::string>();

        auto reliquaryOrigin = ReliquaryOrigin()
            .Register<Shard>()
            .Register<Relic>()
            .Register<GlobalRelic>()
            .Register<Curator>()
            .RelicStructure(relicStructureName, RelicStructure{});

        WHEN("registering shard again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Register<Shard>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The shard (" + TypeFor<Shard>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(Shard).name() + "\".")
                );
            }
        }

        WHEN("registering relic again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Register<Relic>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The relic (" + TypeFor<Relic>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("registering global relic again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Register<GlobalRelic>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The relic (" + TypeFor<GlobalRelic>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(GlobalRelic).name() + "\".")
                );
            }
        }

        WHEN("registering curator again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Register<Curator>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The curator (" + TypeFor<Curator>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(Curator).name() + "\".")
                );
            }
        }

        WHEN("registering relic structure again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.RelicStructure(relicStructureName, RelicStructure{}),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The relic structure (" + relicStructureName + ") was already registered.")
                );
            }
        }
    }
}