#include <catch.hpp>

#include "EitherBatchTests.h"

#include <Arca/Either.h>
#include <Arca/OpenRelic.h>
#include <Arca/Create.h>
#include <Arca/Destroy.h>

#include "BasicShard.h"

SCENARIO_METHOD(EitherBatchTestsFixture, "default either batch", "[either][batch]")
{
    GIVEN("default either batch")
    {
        Batch<Either<BasicShard>> batch;

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

SCENARIO_METHOD(EitherBatchTestsFixture, "either batch", "[either][batch]")
{
    GIVEN("registered reliquary and relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();
        auto relic = reliquary->Do(Create<OpenRelic>());

        WHEN("creating shard")
        {
            auto createdShard = reliquary->Do(Create<BasicShard>(relic));

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<Either<BasicShard>>();

                THEN("batch contains shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == static_cast<const BasicShard*>(createdShard));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    reliquary->Do(Destroy<BasicShard>(relic.ID()));
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("creating const shard")
        {
            auto createdShard = reliquary->Do(Create<const BasicShard>(relic));

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<Either<BasicShard>>();

                THEN("batch contains shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == static_cast<const BasicShard*>(createdShard));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    reliquary->Do(Destroy<const BasicShard>(relic.ID()));
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<Either<BasicShard>>();

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
                auto createdShard = reliquary->Do(Create<BasicShard>(relic));

                THEN("batch contains derived shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == static_cast<const BasicShard*>(createdShard));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    reliquary->Do(Destroy<BasicShard>(relic.ID()));
                    REQUIRE(batch.IsEmpty());
                }
            }

            WHEN("creating const shard")
            {
                auto createdShard = reliquary->Do(Create<const BasicShard>(relic));

                THEN("batch contains derived shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == static_cast<const BasicShard*>(createdShard));
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    reliquary->Do(Destroy<const BasicShard>(relic.ID()));
                    REQUIRE(batch.IsEmpty());
                }
            }
        }
    }

    GIVEN("registered reliquary with global relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<GlobalRelic>()
            .Actualize();

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<Either<BasicShard>>();

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

SCENARIO_METHOD(EitherBatchTestsFixture, "either shard batch serialization", "[EitherShardBatch][serialization]")
{
    GIVEN("saved reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<LocalRelic>()
            .Register<BasicShard>()
            .Actualize();

        savedReliquary->Do(Create<LocalRelic>{ dataGeneration.Random<int>() });

        {
            auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<LocalRelic>()
                .Register<BasicShard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
            }

            auto batch = loadedReliquary->Batch<Either<BasicShard>>();

            THEN("batch is occupied")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
            }
        }
    }
}