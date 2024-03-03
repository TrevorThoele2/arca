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
    const TypeName Traits<CompositeShardTestsFixture::BasicTypedRelic>::typeName =
        "ReliquaryTestsBasicTypedRelic";
}

SCENARIO_METHOD(CompositeShardTestsFixture, "reliquary composite shards", "[reliquary][shard][composite]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Shard<0>>()
            .Type<Shard<1>>()
            .Type<Shard<2>>()
            .Type<Shard<3>>()
            .Type<BasicTypedRelic>()
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
            relic->Create<Shard<0>>();
            relic->Create<Shard<1>>();
            relic->Create<Shard<2>>();

            THEN("contains composite")
            {
                REQUIRE(reliquary->Contains<Shard<0>, Shard<1>, Shard<2>>(relic->ID()));
            }

            WHEN("removing a shard")
            {
                relic->Destroy<Shard<0>>();

                THEN("does not contain composite")
                {
                    REQUIRE(!reliquary->Contains<Shard<0>, Shard<1>, Shard<2>>(relic->ID()));
                }
            }

            WHEN("adding a new shard")
            {
                relic->Create<Shard<3>>();

                THEN("contains composite")
                {
                    REQUIRE(reliquary->Contains<Shard<0>, Shard<1>, Shard<2>>(relic->ID()));
                }

                WHEN("removing that shard")
                {
                    relic->Destroy<Shard<3>>();

                    THEN("contains composite")
                    {
                        REQUIRE(reliquary->Contains<Shard<0>, Shard<1>, Shard<2>>(relic->ID()));
                    }
                }
            }
        }

        WHEN("creating separate relic with shards")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<Shard<0>>();
            relic->Create<Shard<1>>();
            relic->Create<Shard<2>>();

            THEN("irrelevant relic does not contain composite")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
                REQUIRE(!reliquary->Contains<Shard<0>, Shard<1>, Shard<2>>(irrelevant->ID()));
            }
        }
    }
}

SCENARIO_METHOD(CompositeShardTestsFixture, "OpenRelic composite shards", "[OpenRelic][shard][composite]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Shard<0>>()
            .Type<Shard<1>>()
            .Type<Shard<2>>()
            .Type<Shard<3>>()
            .Type<BasicTypedRelic>()
            .Actualize();

        WHEN("creating composite shards")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<Shard<0>>();
            relic->Create<Shard<1>>();
            relic->Create<Shard<2>>();

            THEN("contains composite")
            {
                REQUIRE(relic->Contains<Shard<0>, Shard<1>, Shard<2>>());
            }

            WHEN("removing a shard")
            {
                relic->Destroy<Shard<0>>();

                THEN("does not contain composite")
                {
                    REQUIRE(!relic->Contains<Shard<0>, Shard<1>, Shard<2>>());
                }
            }

            WHEN("adding a new shard")
            {
                relic->Create<Shard<3>>();

                THEN("contains composite")
                {
                    REQUIRE(relic->Contains<Shard<0>, Shard<1>, Shard<2>>());
                }

                WHEN("removing that shard")
                {
                    relic->Destroy<Shard<3>>();

                    THEN("contains composite")
                    {
                        REQUIRE(relic->Contains<Shard<0>, Shard<1>, Shard<2>>());
                    }
                }
            }
        }

        WHEN("creating separate relic with shards")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<Shard<0>>();
            relic->Create<Shard<1>>();
            relic->Create<Shard<2>>();

            THEN("irrelevant relic does not contain composite")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
                REQUIRE(!irrelevant->Contains<Shard<0>, Shard<1>, Shard<2>>());
            }
        }
    }
}

SCENARIO_METHOD(CompositeShardTestsFixture, "ClosedRelic composite shards", "[ClosedRelic][shard][composite]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Shard<0>>()
            .Type<Shard<1>>()
            .Type<Shard<2>>()
            .Type<BasicTypedRelic>()
            .Actualize();

        WHEN("creating const shard")
        {
            auto relic = reliquary->CreateWith<ClosedRelic>(
                RelicStructure
                {
                    TypeFor<Shard<0>>(),
                    TypeFor<Shard<1>>(),
                    TypeFor<Shard<2>>()
                });

            THEN("contains composite")
            {
                REQUIRE(relic->Contains<Shard<0>, Shard<1>, Shard<2>>());
            }
        }

        WHEN("creating separate relic with shard")
        {
            reliquary->CreateWith<ClosedRelic>(
                RelicStructure
                {
                    TypeFor<Shard<0>>(),
                    TypeFor<Shard<1>>(),
                    TypeFor<Shard<2>>()
                });

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->CreateWith<ClosedRelic>(RelicStructure{});
                REQUIRE(!irrelevant->Contains<Shard<0>, Shard<1>, Shard<2>>());
            }
        }
    }
}