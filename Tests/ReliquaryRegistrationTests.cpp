#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryRegistrationTests.h"

namespace Arca
{
    const TypeHandleName Traits<::ReliquaryRegistrationTestsFixture::Shard>::typeName =
        "ReliquaryTestsShard";

    const TypeHandleName Traits<::ReliquaryRegistrationTestsFixture::Relic>::typeName =
        "ReliquaryTestsRelic";

    const TypeHandleName Traits<::ReliquaryRegistrationTestsFixture::GlobalRelic>::typeName =
        "ReliquaryTestsGlobalRelic";

    const TypeHandleName Traits<::ReliquaryRegistrationTestsFixture::Curator>::typeName =
        "ReliquaryTestsCurator";

    const TypeHandleName Traits<::ReliquaryRegistrationTestsFixture::Signal>::typeName =
        "ReliquaryTestsSignal";
}

SCENARIO_METHOD(ReliquaryRegistrationTestsFixture, "registering nothing", "[reliquary]")
{
    GIVEN("no types registered to origin")
    {
        const auto relicStructureName = dataGeneration.Random<std::string>();

        auto reliquary = ReliquaryOrigin().Actualize();

        auto openRelic = reliquary->Create<OpenRelic>();

        WHEN("creating unregistered shard on open relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    openRelic.Create<Shard>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The shard ("s + ::Chroma::ToString(TypeHandleFor<Shard>()) + ") was not registered. " +
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
                    openRelic.Create<const Shard>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The shard ("s + ::Chroma::ToString(TypeHandleFor<const Shard>()) + ") was not registered. " +
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
                    reliquary->Create<Relic>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The relic ("s + ::Chroma::ToString(TypeHandleFor<Relic>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("retrieving unregistered global relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Global<GlobalRelic>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The relic ("s + ::Chroma::ToString(TypeHandleFor<GlobalRelic>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(GlobalRelic).name() + "\".")
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
                        "The signal ("s + ::Chroma::ToString(TypeHandleFor<Signal>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Signal).name() + "\".")
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
                        "The shard (" + ::Chroma::ToString(TypeHandleFor<Shard>()) + ") was not registered. " +
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
                        "The shard (" + ::Chroma::ToString(TypeHandleFor<const Shard>()) + ") was not registered. " +
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
                        "The relic (" + ::Chroma::ToString(TypeHandleFor<Relic>()) + ") was not registered. " +
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
                        "The signal ("s + ::Chroma::ToString(TypeHandleFor<Signal>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Signal).name() + "\".")
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
                        "The shard ("s + ::Chroma::ToString(TypeHandleFor<Shard>()) + ") was not registered.")
                );
            }
        }
    }
}