#include <catch.hpp>

#include "EitherTests.h"

EitherTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

void EitherTestsFixture::BasicTypedRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

SCENARIO_METHOD(EitherTestsFixture, "reliquary either", "[reliquary][either][shard]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Type<BasicTypedRelic>()
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
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<const BasicShard>();

            THEN("contains either")
            {
                REQUIRE(reliquary->Contains<Either<BasicShard>>(relic->ID()));
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<BasicShard>();

            THEN("contains either")
            {
                REQUIRE(reliquary->Contains<Either<BasicShard>>(relic->ID()));
            }
        }

        WHEN("creating const and non-const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<const BasicShard>();
            relic->Create<BasicShard>();

            THEN("contains either")
            {
                REQUIRE(reliquary->Contains<Either<BasicShard>>(relic->ID()));
            }
        }

        WHEN("creating separate relic with shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<BasicShard>();

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
                REQUIRE(!reliquary->Contains<Either<BasicShard>>(irrelevant->ID()));
            }
        }
    }
}

SCENARIO_METHOD(EitherTestsFixture, "OpenRelic either", "[OpenRelic][either][shard]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Type<BasicTypedRelic>()
            .Actualize();

        WHEN("creating const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<const BasicShard>();

            THEN("contains either")
            {
                REQUIRE(relic->Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<BasicShard>();

            THEN("contains either")
            {
                REQUIRE(relic->Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating const and non-const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<const BasicShard>();
            relic->Create<BasicShard>();

            THEN("contains either")
            {
                REQUIRE(relic->Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating separate relic with shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic->Create<BasicShard>();

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
                REQUIRE(!irrelevant->Contains<Either<BasicShard>>());
            }
        }
    }
}

SCENARIO_METHOD(EitherTestsFixture, "ClosedRelic either", "[ClosedRelic][either][shard]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Type<BasicTypedRelic>()
            .Actualize();

        WHEN("creating const shard")
        {
            auto relic = reliquary->CreateWith<ClosedRelic>(RelicStructure { TypeFor<const BasicShard>() });

            THEN("contains either")
            {
                REQUIRE(relic->Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->CreateWith<ClosedRelic>(RelicStructure{ TypeFor<BasicShard>() });

            THEN("contains either")
            {
                REQUIRE(relic->Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating const and non-const shard")
        {
            auto relic = reliquary->CreateWith<ClosedRelic>(
                RelicStructure
                {
                    TypeFor<const BasicShard>(),
                    TypeFor<BasicShard>()
                }
            );

            THEN("contains either")
            {
                REQUIRE(relic->Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating separate relic with shard")
        {
            reliquary->CreateWith<ClosedRelic>(RelicStructure{ TypeFor<BasicShard>() });

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->CreateWith<ClosedRelic>(RelicStructure{});
                REQUIRE(!irrelevant->Contains<Either<BasicShard>>());
            }
        }
    }
}