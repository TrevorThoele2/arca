#include <catch.hpp>

#include "CompositeShardBatchTests.h"

#include <Arca/CompositeShardBatch.h>
#include <Arca/CompositeShardBatchSourceDefinition.h>

namespace Arca
{
    const TypeHandleName Traits<CompositeShardBatchFixture::Relic>::typeName
        = "EitherShardBatchTestsRelic";
}

SCENARIO_METHOD(CompositeShardBatchFixture, "default composite shards batch", "[CompositeShardBatch]")
{
    GIVEN("default composite shards batch")
    {
        Batch<All<Shard<0>, Shard<1>, Shard<2>>> batch;

        WHEN("querying size")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.Size(), BatchNotSetup);
            }
        }

        WHEN("querying empty")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.IsEmpty(), BatchNotSetup);
            }
        }

        WHEN("querying begin")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.begin(), BatchNotSetup);
            }
        }

        WHEN("querying begin const")
        {
            const auto& constBatch = batch;

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(constBatch.begin(), BatchNotSetup);
            }
        }

        WHEN("querying end")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.end(), BatchNotSetup);
            }
        }

        WHEN("querying begin const")
        {
            const auto& constBatch = batch;

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(constBatch.end(), BatchNotSetup);
            }
        }
    }
}

SCENARIO_METHOD(CompositeShardBatchFixture, "composite shard batch", "[CompositeShardBatch]")
{
    GIVEN("registered reliquary and relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard<0>>()
            .Shard<Shard<1>>()
            .Shard<Shard<2>>()
            .Actualize();
        auto relic = reliquary->Create<OpenRelic>();

        WHEN("creating shards")
        {
            auto createdShard0 = relic.Create<Shard<0>>();
            auto createdShard1 = relic.Create<Shard<1>>();
            auto createdShard2 = relic.Create<Shard<2>>();

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<All<Shard<0>, Shard<1>, Shard<2>>>();

                THEN("batch is not empty")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard contains each created shard")
                {
                    auto& first = *batch.begin();
                    REQUIRE(std::get<0>(first) == createdShard0);
                    REQUIRE(std::get<1>(first) == createdShard1);
                    REQUIRE(std::get<2>(first) == createdShard2);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing one shard empties the batch")
                {
                    relic.Destroy<Shard<0>>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<All<Shard<0>, Shard<1>, Shard<2>>>();

            THEN("batch is empty")
            {
                REQUIRE(batch.IsEmpty());
                REQUIRE(batch.Size() == 0);
            }

            THEN("begin is end")
            {
                REQUIRE(batch.begin() == batch.end());
            }

            WHEN("creating shards")
            {
                auto createdShard0 = relic.Create<Shard<0>>();
                auto createdShard1 = relic.Create<Shard<1>>();
                auto createdShard2 = relic.Create<Shard<2>>();

                THEN("batch is not empty")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard contains each created shard")
                {
                    auto& first = *batch.begin();
                    REQUIRE(std::get<0>(first) == createdShard0);
                    REQUIRE(std::get<1>(first) == createdShard1);
                    REQUIRE(std::get<2>(first) == createdShard2);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing one shard empties the batch")
                {
                    relic.Destroy<Shard<0>>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }
    }

    GIVEN("registered reliquary with global relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard<0>>()
            .Shard<Shard<1>>()
            .Shard<Shard<2>>()
            .GlobalRelic<Relic>()
            .Actualize();

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<All<Shard<0>, Shard<1>, Shard<2>>>();

            THEN("batch contains shard")
            {
                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
            }

            THEN("begin is not end")
            {
                REQUIRE(batch.begin() != batch.end());
            }
        }
    }
}