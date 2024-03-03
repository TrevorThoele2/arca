#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryRegistrationTests.h"

namespace Arca
{
    const TypeHandle Traits<::ReliquaryRegistrationTestsFixture::Shard>::typeHandle =
        "ReliquaryTestsBasicShard";

    const TypeHandle Traits<::ReliquaryRegistrationTestsFixture::Relic>::typeHandle =
        "ReliquaryTestsBasicTypedRelic";

    const TypeHandle Traits<::ReliquaryRegistrationTestsFixture::StaticRelic>::typeHandle =
        "ReliquaryTestsStaticRelic";

    const TypeHandle Traits<::ReliquaryRegistrationTestsFixture::Curator>::typeHandle =
        "ReliquaryTestsBasicCurator";
}

SCENARIO_METHOD(ReliquaryRegistrationTestsFixture, "registering nothing", "[reliquary]")
{
    GIVEN("no types registered to origin")
    {
        const auto relicStructureName = dataGeneration.Random<std::string>();

        auto reliquary = ReliquaryOrigin().Actualize();

        auto dynamicRelic = reliquary->Create<DynamicRelic>();

        WHEN("creating unregistered shard on dynamic relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    dynamicRelic.Create<Shard>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The shard ("s + TypeHandleFor<Shard>() + ") was not registered. " +
                        "The class name is: \"" + typeid(Shard).name() + "\".")
                );
            }
        }

        WHEN("creating unregistered typed relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Create<Relic>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The relic ("s + TypeHandleFor<Relic>() + ") was not registered. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("retrieving unregistered static relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Static<StaticRelic>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The static relic ("s + TypeHandleFor<StaticRelic>() + ") was not registered. " +
                        "The class name is: \"" + typeid(StaticRelic).name() + "\".")
                );
            }
        }

        WHEN("raising unregistered signal")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Raise(Signal{}),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The signal ("s + typeid(Signal).name() + ") was not registered.")
                );
            }
        }

        WHEN("retrieving unregistered curator")
        {
            THEN("returns nullptr")
            {
                REQUIRE(reliquary->Find<Curator>() == nullptr);
            }
        }

        WHEN("creating relic with unregistered relic structure")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Create<FixedRelic>(relicStructureName),
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
                        "The shard (" + TypeHandleFor<Shard>() + ") was not registered. " +
                        "The class name is: \"" + typeid(Shard).name() + "\".")
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
                        "The relic (" + TypeHandleFor<Relic>() + ") was not registered. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("retrieving signal batch")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Batch<Signal>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The signal ("s + typeid(Signal).name() + ") was not registered.")
                );
            }
        }
    }

    GIVEN("only typed relic registered to reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<Relic>()
            .Actualize();

        WHEN("creating typed relic with unregistered shard")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Create<Relic>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The shard ("s + TypeHandleFor<Shard>() + ") was not registered.")
                );
            }
        }
    }
}