#include <catch.hpp>

#include "EitherBatchTests.h"

#include <Arca/Either.h>

#include "BasicShard.h"

EitherBatchTestsFixture::Relic::Relic(Init init) : ClosedTypedRelic(init)
{
    Create<BasicShard>();
}

EitherBatchTestsFixture::GlobalRelic::GlobalRelic(Init init) : ClosedTypedRelic(init)
{
    Create<BasicShard>();
}

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
        auto reliquary = ReliquaryOrigin().Register<BasicShard>().Actualize();
        auto relic = reliquary->Do<Create<OpenRelic>>();

        WHEN("creating shard")
        {
            auto createdShard = relic->Create<BasicShard>();

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
                    relic->Destroy<BasicShard>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("creating const shard")
        {
            auto createdShard = relic->Create<const BasicShard>();

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
                    relic->Destroy<const BasicShard>();
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
                auto createdShard = relic->Create<BasicShard>();

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
                    relic->Destroy<BasicShard>();
                    REQUIRE(batch.IsEmpty());
                }
            }

            WHEN("creating const shard")
            {
                auto createdShard = relic->Create<const BasicShard>();

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
                    relic->Destroy<const BasicShard>();
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
            .Register<Relic>()
            .Register<BasicShard>()
            .Actualize();

        savedReliquary->Do<Create<Relic>>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat");
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<Relic>()
                .Register<BasicShard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat");
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