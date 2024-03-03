#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryRegistrationTests.h"

ReliquaryRegistrationTestsFixture::Relic::Relic(Init init) : ClosedTypedRelic(init)
{
    Create<Shard>();
}

SCENARIO_METHOD(ReliquaryRegistrationTestsFixture, "registering nothing", "[reliquary]")
{
    GIVEN("no types registered to origin")
    {
        const auto relicStructureName = dataGeneration.Random<std::string>();

        auto reliquary = ReliquaryOrigin().Actualize();

        auto openRelic = reliquary->Do<Create<OpenRelic>>();

        WHEN("creating unregistered shard on open relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    openRelic->Create<Shard>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The shard ("s + ::Chroma::ToString(TypeFor<Shard>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Shard).name() + "\".")
                );
            }
        }

        WHEN("creating unregistered const shard on open relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    openRelic->Create<const Shard>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The shard ("s + ::Chroma::ToString(TypeFor<const Shard>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(const Shard).name() + "\".")
                );
            }
        }

        WHEN("creating unregistered typed relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do<Create<Relic>>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The relic ("s + ::Chroma::ToString(TypeFor<Relic>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("retrieving unregistered curator")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Find<Curator>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The curator ("s + ::Chroma::ToString(TypeFor<Curator>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Curator).name() + "\".")
                );
            }
        }

        WHEN("creating relic with unregistered relic structure")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do<CreateWith<ClosedRelic>>(relicStructureName),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The relic structure ("s + relicStructureName + ") was not registered.")
                );
            }
        }

        WHEN("retrieving shard batch")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Batch<Shard>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The shard (" + ::Chroma::ToString(TypeFor<Shard>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Shard).name() + "\".")
                );
            }
        }

        WHEN("retrieving const shard batch")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Batch<const Shard>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The shard (" + ::Chroma::ToString(TypeFor<const Shard>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(const Shard).name() + "\".")
                );
            }
        }

        WHEN("retrieving relic batch")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Batch<Relic>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The relic (" + ::Chroma::ToString(TypeFor<Relic>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("executing command")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do<Command>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The command (" + ::Chroma::ToString(TypeFor<Command>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Command).name() + "\".")
                );
            }
        }
    }
}