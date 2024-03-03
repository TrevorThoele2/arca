#include <catch.hpp>

#include "CompositeShardTests.h"

void CompositeShardTestsFixture::BasicTypedRelic::InitializeImplementation()
{
    auto shards = ExtractShards();
    shard0 = std::get<0>(shards);
    shard1 = std::get<1>(shards);
    shard2 = std::get<2>(shards);
}

namespace Arca
{
    const TypeHandleName Traits<CompositeShardTestsFixture::BasicTypedRelic>::typeName =
        "ReliquaryTestsBasicTypedRelic";
}

SCENARIO_METHOD(CompositeShardTestsFixture, "reliquary composite shards", "[reliquary][shard][composite]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard<0>>()
            .Shard<Shard<1>>()
            .Shard<Shard<2>>()
            .Relic<BasicTypedRelic>()
            .Actualize();

        WHEN("not creating anything")
        {
            THEN("does not contain")
            {
                REQUIRE(!reliquary->Contains<Shard<0>, Shard<1>, Shard<2>>(1));
            }
        }

        WHEN("creating composite shards")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<Shard<0>>();
            relic.Create<Shard<1>>();
            relic.Create<Shard<2>>();

            THEN("contains composite")
            {
                REQUIRE(reliquary->Contains<Shard<0>, Shard<1>, Shard<2>>(relic.ID()));
            }
        }

        WHEN("creating separate relic with shards")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<Shard<0>>();
            relic.Create<Shard<1>>();
            relic.Create<Shard<2>>();

            THEN("irrelevant relic does not contain composite")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
                REQUIRE(!reliquary->Contains<Shard<0>, Shard<1>, Shard<2>>(irrelevant.ID()));
            }
        }
    }
}

SCENARIO_METHOD(CompositeShardTestsFixture, "OpenRelic composite shards", "[OpenRelic][shard][composite]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard<0>>()
            .Shard<Shard<1>>()
            .Shard<Shard<2>>()
            .Relic<BasicTypedRelic>()
            .Actualize();

        WHEN("creating composite shards")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<Shard<0>>();
            relic.Create<Shard<1>>();
            relic.Create<Shard<2>>();

            THEN("contains composite")
            {
                REQUIRE(relic.Contains<Shard<0>, Shard<1>, Shard<2>>());
            }
        }

        WHEN("creating separate relic with shards")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<Shard<0>>();
            relic.Create<Shard<1>>();
            relic.Create<Shard<2>>();

            THEN("irrelevant relic does not contain composite")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
                REQUIRE(!irrelevant.Contains<Shard<0>, Shard<1>, Shard<2>>());
            }
        }
    }
}

SCENARIO_METHOD(CompositeShardTestsFixture, "FixedRelic composite shards", "[FixedRelic][shard][composite]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard<0>>()
            .Shard<Shard<1>>()
            .Shard<Shard<2>>()
            .Relic<BasicTypedRelic>()
            .Actualize();

        WHEN("creating const shard")
        {
            auto relic = reliquary->Create<FixedRelic>(
                RelicStructure
                {
                    TypeHandleFor<Shard<0>>(),
                    TypeHandleFor<Shard<1>>(),
                    TypeHandleFor<Shard<2>>()
                });

            THEN("contains composite")
            {
                REQUIRE(relic.Contains<Shard<0>, Shard<1>, Shard<2>>());
            }
        }

        WHEN("creating separate relic with shard")
        {
            reliquary->Create<FixedRelic>(
                RelicStructure
                {
                    TypeHandleFor<Shard<0>>(),
                    TypeHandleFor<Shard<1>>(),
                    TypeHandleFor<Shard<2>>()
                });

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Create<FixedRelic>(RelicStructure{});
                REQUIRE(!irrelevant.Contains<Shard<0>, Shard<1>, Shard<2>>());
            }
        }
    }
}