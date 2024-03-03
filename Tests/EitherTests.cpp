#include <catch.hpp>

#include "EitherTests.h"

EitherTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

void EitherTestsFixture::BasicTypedRelic::InitializeImplementation()
{
    auto shards = ExtractShards();
    basicShard = std::get<0>(shards);
}

namespace Arca
{
    const TypeHandleName Traits<EitherTestsFixture::BasicShard>::typeName =
        "RelicTestsBasicShard";

    const TypeHandleName Traits<EitherTestsFixture::BasicTypedRelic>::typeName =
        "ReliquaryTestsBasicTypedRelic";
}

SCENARIO_METHOD(EitherTestsFixture, "reliquary either", "[reliquary][either][shard]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Relic<BasicTypedRelic>()
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
            relic.Create<const BasicShard>();

            THEN("contains either")
            {
                REQUIRE(reliquary->Contains<Either<BasicShard>>(relic.ID()));
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<BasicShard>();

            THEN("contains either")
            {
                REQUIRE(reliquary->Contains<Either<BasicShard>>(relic.ID()));
            }
        }

        WHEN("creating const and non-const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<const BasicShard>();
            relic.Create<BasicShard>();

            THEN("contains either")
            {
                REQUIRE(reliquary->Contains<Either<BasicShard>>(relic.ID()));
            }
        }

        WHEN("creating separate relic with shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<BasicShard>();

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
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
            .Shard<BasicShard>()
            .Relic<BasicTypedRelic>()
            .Actualize();

        WHEN("creating const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<const BasicShard>();

            THEN("contains either")
            {
                REQUIRE(relic.Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<BasicShard>();

            THEN("contains either")
            {
                REQUIRE(relic.Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating const and non-const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<const BasicShard>();
            relic.Create<BasicShard>();

            THEN("contains either")
            {
                REQUIRE(relic.Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating separate relic with shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            relic.Create<BasicShard>();

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Create<OpenRelic>();
                REQUIRE(!irrelevant.Contains<Either<BasicShard>>());
            }
        }
    }
}

SCENARIO_METHOD(EitherTestsFixture, "FixedRelic either", "[FixedRelic][either][shard]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Relic<BasicTypedRelic>()
            .Actualize();

        WHEN("creating const shard")
        {
            auto relic = reliquary->Create<FixedRelic>(RelicStructure { TypeHandleFor<const BasicShard>() });

            THEN("contains either")
            {
                REQUIRE(relic.Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating non-const shard")
        {
            auto relic = reliquary->Create<FixedRelic>(RelicStructure{ TypeHandleFor<BasicShard>() });

            THEN("contains either")
            {
                REQUIRE(relic.Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating const and non-const shard")
        {
            auto relic = reliquary->Create<FixedRelic>(
                RelicStructure
                {
                    TypeHandleFor<const BasicShard>(),
                    TypeHandleFor<BasicShard>()
                }
            );

            THEN("contains either")
            {
                REQUIRE(relic.Contains<Either<BasicShard>>());
            }
        }

        WHEN("creating separate relic with shard")
        {
            reliquary->Create<FixedRelic>(RelicStructure{ TypeHandleFor<BasicShard>() });

            THEN("irrelevant relic does not contain either")
            {
                auto irrelevant = reliquary->Create<FixedRelic>(RelicStructure{});
                REQUIRE(!irrelevant.Contains<Either<BasicShard>>());
            }
        }
    }
}