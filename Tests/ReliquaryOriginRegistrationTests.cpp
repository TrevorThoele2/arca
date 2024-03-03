#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryOriginRegistrationTests.h"

namespace Arca
{
    const TypeHandleName Traits<::ReliquaryOriginRegistrationTestsFixture::Shard>::typeName =
        "ReliquaryTestsShard";

    const TypeHandleName Traits<::ReliquaryOriginRegistrationTestsFixture::Relic>::typeName =
        "ReliquaryTestsRelic";

    const TypeHandleName Traits<::ReliquaryOriginRegistrationTestsFixture::StaticRelic>::typeName =
        "ReliquaryTestsStaticRelic";

    const TypeHandleName Traits<::ReliquaryOriginRegistrationTestsFixture::Curator>::typeName =
        "ReliquaryTestsCurator";

    const TypeHandleName Traits<::ReliquaryOriginRegistrationTestsFixture::Signal>::typeName =
        "ReliquaryTestsSignal";
}

SCENARIO_METHOD(ReliquaryOriginRegistrationTestsFixture, "registering types multiple times", "[reliquaryorigin]")
{
    GIVEN("all types registered to origin")
    {
        const auto relicStructureName = dataGeneration.Random<std::string>();

        auto reliquaryOrigin = ReliquaryOrigin()
            .Shard<Shard>()
            .Relic<Relic>()
            .StaticRelic<StaticRelic>()
            .Signal<Signal>()
            .Curator<Curator>()
            .RelicStructure(relicStructureName, RelicStructure{});

        WHEN("registering shard again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Shard<Shard>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The shard (" + TypeHandleFor<Shard>().name + ") was already registered. " +
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
                    reliquaryOrigin.Relic<Relic>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The relic (" + TypeHandleFor<Relic>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("registering static relic again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.StaticRelic<StaticRelic>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The static relic (" + TypeHandleFor<StaticRelic>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(StaticRelic).name() + "\".")
                );
            }
        }

        WHEN("registering curator again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Curator<Curator>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The curator (" + TypeHandleFor<Curator>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(Curator).name() + "\".")
                );
            }
        }

        WHEN("registering signal again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Signal<Signal>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The signal ("s + typeid(Signal).name() + ") was already registered.")
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