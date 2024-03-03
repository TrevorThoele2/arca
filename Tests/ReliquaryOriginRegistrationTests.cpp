#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryOriginRegistrationTests.h"

namespace Arca
{
    const TypeName Traits<::ReliquaryOriginRegistrationTestsFixture::Shard>::typeName =
        "ReliquaryTestsShard";

    const TypeName Traits<::ReliquaryOriginRegistrationTestsFixture::Relic>::typeName =
        "ReliquaryTestsRelic";

    const TypeName Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic>::typeName =
        "ReliquaryTestsGlobalRelic";

    const TypeName Traits<::ReliquaryOriginRegistrationTestsFixture::Curator>::typeName =
        "ReliquaryTestsCurator";

    const TypeName Traits<::ReliquaryOriginRegistrationTestsFixture::Signal>::typeName =
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
                    reliquaryOrigin.Type<Relic>(),
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
                    reliquaryOrigin.Type<GlobalRelic>(),
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
                    reliquaryOrigin.Type<Curator>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The curator (" + TypeFor<Curator>().name + ") was already registered. " +
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