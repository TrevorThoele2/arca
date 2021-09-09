#include <catch.hpp>

#include "EitherTests.h"
#include <Arca/OpenRelic.h>

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
                REQUIRE(reliquary->Find<Either<BasicShard>>(relic.ID()));
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<BasicShard>(relic));

            THEN("contains either")
            {
                REQUIRE(reliquary->Find<Either<BasicShard>>(relic.ID()));
            }
        }

        WHEN("creating separate relic with shard")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<BasicShard>(relic));

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Do(Create<OpenRelic>());
                REQUIRE(!reliquary->Find<Either<BasicShard>>(irrelevant.ID()));
            }
        }
    }
}