#include <catch.hpp>

#include "AllTests.h"

#include <Arca/OpenRelic.h>

AllTestsFixture::BasicTypedRelic::BasicTypedRelic(RelicInit init)
{
    shard0 = init.Create<DifferentiableShard<0>>();
    shard1 = init.Create<DifferentiableShard<1>>();
    shard2 = init.Create<DifferentiableShard<2>>();
}

SCENARIO_METHOD(AllTestsFixture, "all", "[all]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Register<DifferentiableShard<3>>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("not creating anything")
        {
            THEN("does not contain")
            {
                auto contains = reliquary->Contains<
                    All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(1);
                REQUIRE(!contains);
            }
        }

        WHEN("creating shards")
        {
            auto relic = reliquary->Do(Create<OpenRelic>{});
            reliquary->Do(Create<DifferentiableShard<0>>(relic));
            reliquary->Do(Create<DifferentiableShard<1>>(relic));
            reliquary->Do(Create<DifferentiableShard<2>>(relic));

            THEN("contains all")
            {
                auto contains = reliquary->Contains<
                    All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(
                        relic.ID());
                REQUIRE(contains);
            }

            WHEN("removing a shard")
            {
                reliquary->Do(Destroy<DifferentiableShard<0>>(relic.ID()));

                THEN("does not contain all")
                {
                    auto contains = reliquary->Contains<
                        All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(relic.ID());
                    REQUIRE(!contains);
                }
            }

            WHEN("adding a new shard")
            {
                reliquary->Do(Create<DifferentiableShard<3>>(relic));

                THEN("contains all")
                {
                    auto index = Arca::Index<
                        All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(
                            relic.ID(), *reliquary);
                    REQUIRE(index);
                }

                WHEN("removing that shard")
                {
                    reliquary->Do(Destroy<DifferentiableShard<3>>(relic.ID()));

                    THEN("contains all")
                    {
                        auto contains = reliquary->Contains
                            <All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(
                                relic.ID());
                        REQUIRE(contains);
                    }
                }
            }
        }

        WHEN("creating separate relic with shards")
        {
            auto relic = reliquary->Do(Create<OpenRelic>{});
            reliquary->Do(Create<DifferentiableShard<0>>(relic));
            reliquary->Do(Create<DifferentiableShard<1>>(relic));
            reliquary->Do(Create<DifferentiableShard<2>>(relic));

            THEN("irrelevant relic does not contain all")
            {
                auto irrelevant = reliquary->Do(Create<OpenRelic>{});
                auto contains = reliquary->Contains<
                    All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(irrelevant.ID());
                REQUIRE(!contains);
            }
        }
    }
}

SCENARIO_METHOD(AllTestsFixture, "OpenRelic all", "[OpenRelic][all]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Register<DifferentiableShard<3>>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("creating shards")
        {
            auto relic = reliquary->Do(Create<OpenRelic>{});
            reliquary->Do(Create<DifferentiableShard<0>>(relic));
            reliquary->Do(Create<DifferentiableShard<1>>(relic));
            reliquary->Do(Create<DifferentiableShard<2>>(relic));

            THEN("contains all")
            {
                auto index = Arca::Index<
                    All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(
                        relic.ID(), *reliquary);
                REQUIRE(index);
            }

            WHEN("removing a shard")
            {
                reliquary->Do(Destroy<DifferentiableShard<0>>(relic.ID()));

                THEN("does not contain all")
                {
                    auto index = Arca::Index<
                        All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(
                            relic.ID(), *reliquary);
                    REQUIRE(!index);
                }
            }

            WHEN("adding a new shard")
            {
                reliquary->Do(Create<DifferentiableShard<3>>(relic));

                THEN("contains all")
                {
                    auto index = Arca::Index<
                        All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(
                            relic.ID(), *reliquary);
                    REQUIRE(index);
                }

                WHEN("removing that shard")
                {
                    reliquary->Do(Destroy<DifferentiableShard<3>>(relic.ID()));

                    THEN("contains all")
                    {
                        auto index = Arca::Index<
                            All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(
                                relic.ID(), *reliquary);
                        REQUIRE(index);
                    }
                }
            }
        }

        WHEN("creating separate relic with shards")
        {
            auto relic = reliquary->Do(Create<OpenRelic>{});
            reliquary->Do(Create<DifferentiableShard<0>>(relic));
            reliquary->Do(Create<DifferentiableShard<1>>(relic));
            reliquary->Do(Create<DifferentiableShard<2>>(relic));

            THEN("irrelevant relic does not contain all")
            {
                auto irrelevant = reliquary->Do(Create<OpenRelic>{});
                auto index = Arca::Index<
                    All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(
                        irrelevant.ID(), *reliquary);
                REQUIRE(!index);
            }
        }
    }
}