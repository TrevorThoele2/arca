#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryTests.h"

#include <Arca/OpenRelic.h>
#include <Arca/Create.h>
#include <Arca/Either.h>

ReliquaryTestsFixture::BasicTypedRelic::BasicTypedRelic(RelicInit init)
{
    basicShard = init.Create<BasicShard>();
}

ReliquaryTestsFixture::GlobalRelic::GlobalRelic(RelicInit init)
{
    basicShard = init.Create<BasicShard>();
}

SCENARIO_METHOD(ReliquaryTestsFixture, "default reliquary", "[reliquary]")
{
    GIVEN("default reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Actualize();

        WHEN("checking relic size")
        {
            THEN("has size of zero")
            {
                REQUIRE(reliquary->RelicSize() == 0);
            }
        }

        WHEN("checking shard size")
        {
            THEN("has size of zero")
            {
                REQUIRE(reliquary->ShardSize() == 0);
            }
        }

        WHEN("checking curator size")
        {
            THEN("has size of zero")
            {
                REQUIRE(reliquary->CuratorSize() == 0);
            }
        }

        WHEN("finding relic")
        {
            auto found = reliquary->Find<OpenRelic>(1);

            THEN("is empty")
            {
                REQUIRE(!found);
            }
        }

        WHEN("querying contains relic")
        {
            auto contains = reliquary->Contains<OpenRelic>(1);

            THEN("is false")
            {
                REQUIRE(!contains);
            }
        }

        WHEN("creating relic")
        {
            reliquary->Do(Create<OpenRelic>());

            THEN("reliquary has relic count of one")
            {
                REQUIRE(reliquary->RelicSize() == 1);
            }
        }

        WHEN("finding curator")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary->Find<BasicCurator>(), NotRegistered);
            }
        }

        WHEN("finding curator const")
        {
            const auto& constReliquary = reliquary;

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(constReliquary->Find<BasicCurator>(), NotRegistered);
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "reliquary wih single shard")
{
    GIVEN("reliquary registered with single shard")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        WHEN("creating open relic with non-const shard")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Create<BasicShard>(relic.ID()));

            THEN("creating const shard of same type throws")
            {
                REQUIRE_THROWS_AS(reliquary->Do(Create<const BasicShard>(relic.ID())), CannotCreate);
            }

            THEN("finding either shard gives correct")
            {
                auto found = relic.Owner()->Find<Either<BasicShard>>(relic.ID());
                REQUIRE(found);
                REQUIRE(&*found == &*shard);
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "creating relic from registered relic structure", "[reliquary]")
{
    GIVEN("reliquary registered with structure")
    {
        const auto structureName = dataGeneration.Random<std::string>();
        const auto relicStructure = RelicStructure{ TypeFor<BasicShard>() };

        auto reliquaryOrigin = ReliquaryOrigin()
            .RelicStructure(structureName, relicStructure);

        WHEN("constructing relic from registered structure")
        {
            auto reliquary = reliquaryOrigin
                .Register<OpenRelic>()
                .Register<BasicShard>()
                .Actualize();

            auto relic = reliquary->Do(Create<OpenRelic>{CreateData{ .structure = structureName }});

            THEN("relic has shard")
            {
                REQUIRE(reliquary->Find<BasicShard>(relic.ID()));
            }
        }

        WHEN("constructing relic without registering shard")
        {
            auto reliquary = reliquaryOrigin
                .Register<OpenRelic>()
                .Actualize();

            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(Create<OpenRelic>{CreateData{ .structure = structureName }}),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The shard ("s + Traits<BasicShard>::typeName + ") was not registered.")
                );
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "registered reliquary with every type", "[reliquary]")
{
    GIVEN("all registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<BasicTypedRelic>()
            .Register<GlobalRelic>()
            .RelicStructure(dataGeneration.Random<std::string>(), RelicStructure())
            .Register<BasicShard>()
            .Register<BasicCurator>()
            .CuratorConstructionPipeline(Pipeline())
            .Actualize();

        WHEN("checking relic size")
        {
            THEN("has one relic for global relic")
            {
                REQUIRE(reliquary->RelicSize() == 1);
            }

            THEN("has one shard for global relic")
            {
                REQUIRE(reliquary->ShardSize() == 1);
            }
        }
    }
}