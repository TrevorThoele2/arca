#include <catch.hpp>

#include "EitherShardBatchTests.h"

#include <Arca/EitherShardBatch.h>

EitherShardBatchFixture::Shard::Shard(int value) :
    value(value)
{}

namespace Arca
{
    const TypeHandleName Traits<EitherShardBatchFixture::Shard>::typeName
        = "EitherShardBatchTestsShard";

    const TypeHandleName Traits<EitherShardBatchFixture::Relic>::typeName
        = "EitherShardBatchTestsRelic";
}

SCENARIO_METHOD(EitherShardBatchFixture, "default either shard batch", "[EitherBatch]")
{
    GIVEN("default either batch")
    {
        Batch<Either<Shard>> batch;

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

SCENARIO_METHOD(EitherShardBatchFixture, "either shard batch", "[EitherShardBatch]")
{
    GIVEN("registered reliquary and relic")
    {
        auto reliquary = ReliquaryOrigin().Shard<Shard>().Actualize();
        auto relic = reliquary->Create<OpenRelic>();

        WHEN("creating shard")
        {
            auto createdShard = relic.Create<Shard>();

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<Either<Shard>>();

                THEN("batch contains shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == static_cast<Shard*>(createdShard));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    relic.Destroy<Shard>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("creating const shard")
        {
            auto createdShard = relic.Create<const Shard>();

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<Either<Shard>>();

                THEN("batch contains shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == static_cast<const Shard*>(createdShard));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    relic.Destroy<Shard>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("creating non-const and const shard")
        {
            auto createdShard1 = relic.Create<Shard>();
            auto createdShard2 = relic.Create<const Shard>();

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<Either<Shard>>();

                THEN("batch contains shards")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 2);
                }

                THEN("batch contains entry referentially equal to non-const")
                {
                    REQUIRE(std::any_of(
                        batch.begin(),
                        batch.end(),
                        [&createdShard1](const Shard& shard)
                        {
                            return static_cast<const Shard*>(createdShard1) == &shard;
                        }));
                }

                THEN("batch contains entry referentially equal to const")
                {
                    REQUIRE(std::any_of(
                        batch.begin(),
                        batch.end(),
                        [&createdShard2](const Shard& shard)
                        {
                            return static_cast<const Shard*>(createdShard2) == &shard;
                        }));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing one shard does not empty the batch")
                {
                    relic.Destroy<Shard>();
                    REQUIRE(!batch.IsEmpty());
                }

                THEN("removing both shards empties the batch")
                {
                    relic.Destroy<Shard>();
                    relic.Destroy<const Shard>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<Either<Shard>>();

            THEN("batch is empty")
            {
                REQUIRE(batch.IsEmpty());
                REQUIRE(batch.Size() == 0);
            }

            THEN("begin is end")
            {
                REQUIRE(batch.begin() == batch.end());
            }

            WHEN("creating non-const shard")
            {
                auto createdShard = relic.Create<Shard>();

                THEN("batch contains derived shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == static_cast<Shard*>(createdShard));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    relic.Destroy<Shard>();
                    REQUIRE(batch.IsEmpty());
                }
            }

            WHEN("creating const shard")
            {
                auto createdShard = relic.Create<const Shard>();

                THEN("batch contains derived shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == static_cast<const Shard*>(createdShard));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    relic.Destroy<Shard>();
                    REQUIRE(batch.IsEmpty());
                }
            }

            WHEN("creating non-const and const shard")
            {
                auto createdShard1 = relic.Create<Shard>();
                auto createdShard2 = relic.Create<const Shard>();

                THEN("batch contains shards")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 2);
                }

                THEN("batch contains entry referentially equal to non-const")
                {
                    REQUIRE(std::any_of(
                        batch.begin(),
                        batch.end(),
                        [&createdShard1](const Shard& shard)
                        {
                            return static_cast<const Shard*>(createdShard1) == &shard;
                        }));
                }

                THEN("batch contains entry referentially equal to const")
                {
                    REQUIRE(std::any_of(
                        batch.begin(),
                        batch.end(),
                        [&createdShard2](const Shard& shard)
                        {
                            return static_cast<const Shard*>(createdShard2) == &shard;
                        }));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing one shard does not empty the batch")
                {
                    relic.Destroy<Shard>();
                    REQUIRE(!batch.IsEmpty());
                }

                THEN("removing both shards empties the batch")
                {
                    relic.Destroy<Shard>();
                    relic.Destroy<const Shard>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }
    }

    GIVEN("registered reliquary with global relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .GlobalRelic<Relic>()
            .Actualize();

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<Either<Shard>>();

            THEN("batch contains shard")
            {
                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
            }

            THEN("begin is end")
            {
                REQUIRE(batch.begin() != batch.end());
            }
        }
    }
}