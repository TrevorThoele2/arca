#include <catch.hpp>

#include "AllTests.h"

AllTestsFixture::BasicTypedRelic::BasicTypedRelic(Init init) :
    ClosedTypedRelic(init)
{
    shard0 = Create<DifferentiableShard<0>>();
    shard1 = Create<DifferentiableShard<1>>();
    shard2 = Create<DifferentiableShard<2>>();
}

SCENARIO_METHOD(AllTestsFixture, "all", "[all]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
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
                REQUIRE(!reliquary->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(1));
            }
        }

        WHEN("creating shards")
        {
            auto relic = reliquary->Do(Create<OpenRelic>{});
            relic->Create<DifferentiableShard<0>>();
            relic->Create<DifferentiableShard<1>>();
            relic->Create<DifferentiableShard<2>>();

            THEN("contains all")
            {
                REQUIRE(reliquary->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(relic->ID()));
            }

            WHEN("removing a shard")
            {
                relic->Destroy<DifferentiableShard<0>>();

                THEN("does not contain all")
                {
                    REQUIRE(!reliquary->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(relic->ID()));
                }
            }

            WHEN("adding a new shard")
            {
                relic->Create<DifferentiableShard<3>>();

                THEN("contains all")
                {
                    REQUIRE(reliquary->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(relic->ID()));
                }

                WHEN("removing that shard")
                {
                    relic->Destroy<DifferentiableShard<3>>();

                    THEN("contains all")
                    {
                        REQUIRE(reliquary->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(relic->ID()));
                    }
                }
            }
        }

        WHEN("creating separate relic with shards")
        {
            auto relic = reliquary->Do(Create<OpenRelic>{});
            relic->Create<DifferentiableShard<0>>();
            relic->Create<DifferentiableShard<1>>();
            relic->Create<DifferentiableShard<2>>();

            THEN("irrelevant relic does not contain all")
            {
                auto irrelevant = reliquary->Do(Create<OpenRelic>{});
                REQUIRE(!reliquary->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(irrelevant->ID()));
            }
        }
    }
}

SCENARIO_METHOD(AllTestsFixture, "OpenRelic all", "[OpenRelic][all]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Register<DifferentiableShard<3>>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("creating shards")
        {
            auto relic = reliquary->Do(Create<OpenRelic>{});
            relic->Create<DifferentiableShard<0>>();
            relic->Create<DifferentiableShard<1>>();
            relic->Create<DifferentiableShard<2>>();

            THEN("contains all")
            {
                REQUIRE(relic->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>());
            }

            WHEN("removing a shard")
            {
                relic->Destroy<DifferentiableShard<0>>();

                THEN("does not contain all")
                {
                    REQUIRE(!relic->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>());
                }
            }

            WHEN("adding a new shard")
            {
                relic->Create<DifferentiableShard<3>>();

                THEN("contains all")
                {
                    REQUIRE(relic->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>());
                }

                WHEN("removing that shard")
                {
                    relic->Destroy<DifferentiableShard<3>>();

                    THEN("contains all")
                    {
                        REQUIRE(relic->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>());
                    }
                }
            }
        }

        WHEN("creating separate relic with shards")
        {
            auto relic = reliquary->Do(Create<OpenRelic>{});
            relic->Create<DifferentiableShard<0>>();
            relic->Create<DifferentiableShard<1>>();
            relic->Create<DifferentiableShard<2>>();

            THEN("irrelevant relic does not contain all")
            {
                auto irrelevant = reliquary->Do(Create<OpenRelic>{});
                REQUIRE(!irrelevant->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>());
            }
        }
    }
}

SCENARIO_METHOD(AllTestsFixture, "ClosedRelic all", "[ClosedRelic][all]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("creating shards")
        {
            auto relic = reliquary->Do(CreateWith<ClosedRelic>{
                RelicStructure
                {
                    TypeFor<DifferentiableShard<0>>(),
                    TypeFor<DifferentiableShard<1>>(),
                    TypeFor<DifferentiableShard<2>>()
                } });

            THEN("contains all")
            {
                REQUIRE(relic->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>());
            }
        }

        WHEN("creating separate relic with shard")
        {
            reliquary->Do(CreateWith<ClosedRelic> {
                RelicStructure
                {
                    TypeFor<DifferentiableShard<0>>(),
                    TypeFor<DifferentiableShard<1>>(),
                    TypeFor<DifferentiableShard<2>>()
                } });

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Do(CreateWith<ClosedRelic>{ RelicStructure{} });
                REQUIRE(!irrelevant->Contains<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>());
            }
        }
    }
}