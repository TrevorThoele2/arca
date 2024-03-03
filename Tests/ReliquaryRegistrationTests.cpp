#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryRegistrationTests.h"

namespace Arca
{
    const TypeName Traits<::ReliquaryRegistrationTestsFixture::Shard>::typeName =
        "ReliquaryTestsShard";

    const TypeName Traits<::ReliquaryRegistrationTestsFixture::Relic>::typeName =
        "ReliquaryTestsRelic";

    const TypeName Traits<::ReliquaryRegistrationTestsFixture::GlobalRelic>::typeName =
        "ReliquaryTestsGlobalRelic";

    const TypeName Traits<::ReliquaryRegistrationTestsFixture::GlobalRelicAlias>::typeName =
        "ReliquaryTestsGlobalRelicAlias";

    const TypeName Traits<::ReliquaryRegistrationTestsFixture::Curator>::typeName =
        "ReliquaryTestsCurator";

    const TypeName Traits<::ReliquaryRegistrationTestsFixture::Signal>::typeName =
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
                    reliquary->Create<Relic>(),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The relic ("s + ::Chroma::ToString(TypeFor<Relic>()) + ") was not registered. " +
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
                    Arca::GlobalPtr<GlobalRelic>(*reliquary),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The relic ("s + ::Chroma::ToString(TypeFor<GlobalRelic>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(GlobalRelic).name() + "\".")
                );
            }
        }

        WHEN("retrieving unregistered global alias")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    Arca::AliasPtr<int>(*reliquary),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The alias ("s + typeid(int).name() + ") was not registered.")
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
                        "The signal ("s + ::Chroma::ToString(TypeFor<Signal>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Signal).name() + "\".")
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
                    reliquary->CreateWith<ClosedRelic>(relicStructureName),
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

        WHEN("retrieving signal batch")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Batch<Signal>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The signal ("s + ::Chroma::ToString(TypeFor<Signal>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Signal).name() + "\".")
                );
            }
        }
    }

    GIVEN("only typed relic registered to reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Relic>()
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
                        "The shard ("s + ::Chroma::ToString(TypeFor<Shard>()) + ") was not registered.")
                );
            }
        }
    }
}