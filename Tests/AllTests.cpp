#include <catch.hpp>

#include "AllTests.h"

void AllTestsFixture::BasicTypedRelic::Initialize()
{
    shard0 = Create<Shard<0>>();
    shard1 = Create<Shard<1>>();
    shard2 = Create<Shard<2>>();
}

SCENARIO_METHOD(AllTestsFixture, "all", "[all]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard<0>>()
            .Register<Shard<1>>()
            .Register<Shard<2>>()
            .Register<Shard<3>>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("not creating anything")
        {
            THEN("does not contain")
            {
                REQUIRE(!reliquary->Contains<All<Shard<0>, Shard<1>, Shard<2>>>(1));
            }
        }

        WHEN("creating shards")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<Shard<0>>();
            relic->Create<Shard<1>>();
            relic->Create<Shard<2>>();

            THEN("contains all")
            {
                REQUIRE(reliquary->Contains<All<Shard<0>, Shard<1>, Shard<2>>>(relic->ID()));
            }

            WHEN("removing a shard")
            {
                relic->Destroy<Shard<0>>();

                THEN("does not contain all")
                {
                    REQUIRE(!reliquary->Contains<All<Shard<0>, Shard<1>, Shard<2>>>(relic->ID()));
                }
            }

            WHEN("adding a new shard")
            {
                relic->Create<Shard<3>>();

                THEN("contains all")
                {
                    REQUIRE(reliquary->Contains<All<Shard<0>, Shard<1>, Shard<2>>>(relic->ID()));
                }

                WHEN("removing that shard")
                {
                    relic->Destroy<Shard<3>>();

                    THEN("contains all")
                    {
                        REQUIRE(reliquary->Contains<All<Shard<0>, Shard<1>, Shard<2>>>(relic->ID()));
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

            THEN("irrelevant relic does not contain all")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
                REQUIRE(!reliquary->Contains<All<Shard<0>, Shard<1>, Shard<2>>>(irrelevant->ID()));
            }
        }
    }
}

SCENARIO_METHOD(AllTestsFixture, "OpenRelic all", "[OpenRelic][all]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard<0>>()
            .Register<Shard<1>>()
            .Register<Shard<2>>()
            .Register<Shard<3>>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("creating shards")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<Shard<0>>();
            relic->Create<Shard<1>>();
            relic->Create<Shard<2>>();

            THEN("contains all")
            {
                REQUIRE(relic->Contains<All<Shard<0>, Shard<1>, Shard<2>>>());
            }

            WHEN("removing a shard")
            {
                relic->Destroy<Shard<0>>();

                THEN("does not contain all")
                {
                    REQUIRE(!relic->Contains<All<Shard<0>, Shard<1>, Shard<2>>>());
                }
            }

            WHEN("adding a new shard")
            {
                relic->Create<Shard<3>>();

                THEN("contains all")
                {
                    REQUIRE(relic->Contains<All<Shard<0>, Shard<1>, Shard<2>>>());
                }

                WHEN("removing that shard")
                {
                    relic->Destroy<Shard<3>>();

                    THEN("contains all")
                    {
                        REQUIRE(relic->Contains<All<Shard<0>, Shard<1>, Shard<2>>>());
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

            THEN("irrelevant relic does not contain all")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
                REQUIRE(!irrelevant->Contains<All<Shard<0>, Shard<1>, Shard<2>>>());
            }
        }
    }
}

SCENARIO_METHOD(AllTestsFixture, "ClosedRelic all", "[ClosedRelic][all]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard<0>>()
            .Register<Shard<1>>()
            .Register<Shard<2>>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("creating shards")
        {
            auto relic = reliquary->CreateWith<ClosedRelic>(
                RelicStructure
                {
                    TypeFor<Shard<0>>(),
                    TypeFor<Shard<1>>(),
                    TypeFor<Shard<2>>()
                });

            THEN("contains all")
            {
                REQUIRE(relic->Contains<All<Shard<0>, Shard<1>, Shard<2>>>());
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
                REQUIRE(!irrelevant->Contains<All<Shard<0>, Shard<1>, Shard<2>>>());
            }
        }
    }
}