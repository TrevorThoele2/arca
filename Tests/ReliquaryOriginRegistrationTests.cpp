#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryOriginRegistrationTests.h"

namespace Arca
{
    const TypeHandleName Traits<::ReliquaryOriginRegistrationTestsFixture::Shard>::typeName =
        "ReliquaryTestsShard";

    const TypeHandleName Traits<::ReliquaryOriginRegistrationTestsFixture::Relic>::typeName =
        "ReliquaryTestsRelic";

    const TypeHandleName Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic>::typeName =
        "ReliquaryTestsGlobalRelic";

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
            .Type<Shard>()
            .Type<Relic>()
            .Type<GlobalRelic>()
            .Type<Signal>()
            .Type<Curator>()
            .RelicStructure(relicStructureName, RelicStructure{});

        WHEN("registering shard again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Type<Shard>(),
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
                    reliquaryOrigin.Type<Relic>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The relic (" + TypeHandleFor<Relic>().name + ") was already registered. " +
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
                    reliquaryOrigin.Type<GlobalRelic>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The relic (" + TypeHandleFor<GlobalRelic>().name + ") was already registered. " +
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
                    reliquaryOrigin.Type<Curator>(),
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
                    reliquaryOrigin.Type<Signal>(),
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