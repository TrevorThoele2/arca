#include <catch.hpp>

#include "EitherTests.h"
#include <Arca/LocalRelic.h>

EitherTestsFixture::BasicTypedRelic::BasicTypedRelic(RelicInit init)
{
    basicShard = init.Create<BasicShard>();
}

SCENARIO_METHOD(EitherTestsFixture, "reliquary either", "[reliquary][either][shard]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("not creating anything")
        {
            THEN("does not contain")
            {
                REQUIRE(!reliquary->Contains<Either<BasicShard>>(1));
            }
        }

        WHEN("creating const shard")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<const BasicShard>(relic));

            THEN("contains either")
            {
                REQUIRE(reliquary->Contains<Either<BasicShard>>(relic.ID()));
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<BasicShard>(relic));

            THEN("contains either")
            {
                REQUIRE(reliquary->Contains<Either<BasicShard>>(relic.ID()));
            }
        }

        WHEN("creating separate relic with shard")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<BasicShard>(relic));

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Do(Create<OpenRelic>());
                REQUIRE(!reliquary->Contains<Either<BasicShard>>(irrelevant.ID()));
            }
        }
    }
}

SCENARIO_METHOD(EitherTestsFixture, "OpenRelic either", "[OpenRelic][either][shard]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("creating const shard")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<const BasicShard>(relic));

            THEN("contains either")
            {
                REQUIRE(Arca::Index<Either<BasicShard>>(relic.ID(), *reliquary));
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<BasicShard>(relic));

            THEN("contains either")
            {
                REQUIRE(Arca::Index<Either<BasicShard>>(relic.ID(), *reliquary));
            }
        }

        WHEN("creating separate relic with shard")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<BasicShard>(relic));

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Do(Create<OpenRelic>());
                REQUIRE(!Arca::Index<Either<BasicShard>>(irrelevant.ID(), *reliquary));
            }
        }
    }
}

SCENARIO_METHOD(EitherTestsFixture, "ClosedRelic either", "[ClosedRelic][either][shard]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<ClosedRelic>()
            .Register<BasicShard>()
            .Register<BasicTypedRelic>()
            .Actualize();

        WHEN("creating const shard")
        {
            auto relic = reliquary->Do(CreateWith<ClosedRelic>{ RelicStructure{ TypeFor<const BasicShard>() } });

            THEN("contains either")
            {
                REQUIRE(Arca::Index<Either<BasicShard>>(relic.ID(), *reliquary));
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->Do(CreateWith<ClosedRelic>{ RelicStructure{ TypeFor<BasicShard>() } });

            THEN("contains either")
            {
                REQUIRE(Arca::Index<Either<BasicShard>>(relic.ID(), *reliquary));
            }
        }

        WHEN("creating separate relic with shard")
        {
            reliquary->Do(CreateWith<ClosedRelic>{ RelicStructure{ TypeFor<BasicShard>() } });

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Do(CreateWith<ClosedRelic>{ RelicStructure{} });
                REQUIRE(!Arca::Index<Either<BasicShard>>(irrelevant.ID(), *reliquary));
            }
        }
    }
}