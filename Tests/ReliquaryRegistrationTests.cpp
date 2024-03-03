#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryRegistrationTests.h"

#include <Arca/OpenRelic.h>

ReliquaryRegistrationTestsFixture::Relic::Relic(RelicInit init)
{
    init.Create<Shard>();
}

ReliquaryRegistrationTestsFixture::GlobalRelic::GlobalRelic(RelicInit init)
{}

SCENARIO_METHOD(ReliquaryRegistrationTestsFixture, "registering nothing", "[reliquary]")
{
    GIVEN("no types registered to origin")
    {
        const auto relicStructureName = dataGeneration.Random<std::string>();

        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Actualize();

        auto openRelic = reliquary->Do(Create<OpenRelic>());

        WHEN("creating unregistered shard on open relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(Create<Shard>(openRelic.ID())),
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
                    reliquary->Do(Create<const Shard>(openRelic.ID())),
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
                    reliquary->Do(Create<Relic>()),
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
                    reliquary->Do(CreateWith<OpenRelic>{relicStructureName}),
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
                    reliquary->Do(Command()),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The command (" + ::Chroma::ToString(TypeFor<Command>()) + ") was not registered. " +
                        "The class name is: \"" + typeid(Command).name() + "\".")
                );
            }
        }
    }

    GIVEN("relic types registered to reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<Relic>()
            .Register<GlobalRelic>()
            .Actualize();

        const auto typeNames = reliquary->AllRelicTypeNames();

        WHEN("checking open relic type")
        {
            const auto typeName = TypeFor<OpenRelic>().name;

            THEN("is relic type")
            {
                REQUIRE(reliquary->IsRelicTypeName(typeName));
                const auto found = std::find(
                    typeNames.begin(),
                    typeNames.end(),
                    typeName);
                REQUIRE(found != typeNames.end());
            }

            THEN("is relic object handle type")
            {
                REQUIRE(reliquary->ObjectHandleTypeFor(typeName) == HandleObjectType::Relic);
            }
        }
        
        WHEN("checking typed relic type")
        {
            const auto typeName = TypeFor<Relic>().name;

            THEN("is relic type")
            {
                REQUIRE(reliquary->IsRelicTypeName(typeName));
                const auto found = std::find(
                    typeNames.begin(),
                    typeNames.end(),
                    typeName);
                REQUIRE(found != typeNames.end());
            }

            THEN("is relic object handle type")
            {
                REQUIRE(reliquary->ObjectHandleTypeFor(typeName) == HandleObjectType::Relic);
            }
        }

        WHEN("checking global relic type")
        {
            const auto typeName = TypeFor<GlobalRelic>().name;

            THEN("is relic type")
            {
                REQUIRE(reliquary->IsRelicTypeName(typeName));
                const auto found = std::find(
                    typeNames.begin(),
                    typeNames.end(),
                    typeName);
                REQUIRE(found != typeNames.end());
            }

            THEN("is relic object handle type")
            {
                REQUIRE(reliquary->ObjectHandleTypeFor(typeName) == HandleObjectType::Relic);
            }
        }
    }

    GIVEN("shard types registered to reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Actualize();

        const auto typeNames = reliquary->AllShardTypeNames();

        WHEN("checking shard type")
        {
            const auto typeName = TypeFor<Shard>().name;

            THEN("is shard type")
            {
                REQUIRE(reliquary->IsShardTypeName(typeName));
                const auto found = std::find(
                    typeNames.begin(),
                    typeNames.end(),
                    typeName);
                REQUIRE(found != typeNames.end());
            }

            THEN("is shard object handle type")
            {
                REQUIRE(reliquary->ObjectHandleTypeFor(typeName) == HandleObjectType::Shard);
            }
        }
    }
}