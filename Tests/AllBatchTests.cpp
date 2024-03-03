#include <catch.hpp>

#include "AllBatchTests.h"

AllBatchTestsFixture::Relic::Relic(Initialization initialization) : ClosedTypedRelic(initialization)
{
    Create<Shard<0>>();
    Create<Shard<1>>();
    Create<Shard<2>>();
}

AllBatchTestsFixture::GlobalRelic::GlobalRelic(Initialization initialization) : ClosedTypedRelic(initialization)
{
    Create<Shard<0>>();
    Create<Shard<1>>();
    Create<Shard<2>>();
}

SCENARIO_METHOD(AllBatchTestsFixture, "default all shards batch", "[all][batch]")
{
    GIVEN("default all batch")
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

SCENARIO_METHOD(AllBatchTestsFixture, "all batch", "[all][batch]")
{
    GIVEN("registered reliquary and relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard<0>>()
            .Register<Shard<1>>()
            .Register<Shard<2>>()
            .Register<Shard<3>>()
            .Actualize();
        auto relic = reliquary->Do<Create<OpenRelic>>();

        WHEN("creating shards")
        {
            auto createdShard0 = relic->Create<Shard<0>>();
            auto createdShard1 = relic->Create<Shard<1>>();
            auto createdShard2 = relic->Create<Shard<2>>();

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
                    relic->Destroy<Shard<0>>();
                    REQUIRE(batch.IsEmpty());
                }

                WHEN("adding a new shard")
                {
                    relic->Create<Shard<3>>();

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

                    THEN("removing fourth shard does not empty batch")
                    {
                        relic->Destroy<Shard<3>>();
                        REQUIRE(!batch.IsEmpty());
                    }
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
                auto createdShard0 = relic->Create<Shard<0>>();
                auto createdShard1 = relic->Create<Shard<1>>();
                auto createdShard2 = relic->Create<Shard<2>>();

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
                    relic->Destroy<Shard<0>>();
                    REQUIRE(batch.IsEmpty());
                }

                WHEN("adding a new shard")
                {
                    relic->Create<Shard<3>>();

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

                    THEN("removing fourth shard does not empty batch")
                    {
                        relic->Destroy<Shard<3>>();
                        REQUIRE(!batch.IsEmpty());
                    }
                }
            }
        }
    }

    GIVEN("registered reliquary with global relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard<0>>()
            .Register<Shard<1>>()
            .Register<Shard<2>>()
            .Register<GlobalRelic>()
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

SCENARIO_METHOD(AllBatchTestsFixture, "all batch with either", "[all][batch][either]")
{
    GIVEN("registered reliquary and relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard<0>>()
            .Register<Shard<1>>()
            .Register<Shard<2>>()
            .Register<Shard<3>>()
            .Actualize();
        auto relic = reliquary->Do<Create<OpenRelic>>();

        WHEN("creating shards")
        {
            auto createdShard0 = relic->Create<Shard<0>>();
            auto createdShard1 = relic->Create<Shard<1>>();
            auto createdShard2 = relic->Create<Shard<2>>();

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<All<Either<Shard<0>>, Either<Shard<1>>, Either<Shard<2>>>>();

                THEN("batch is not empty")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned matrix contains each created shard")
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
                    relic->Destroy<Shard<0>>();
                    REQUIRE(batch.IsEmpty());
                }

                WHEN("adding a new relic")
                {
                    relic->Create<Shard<3>>();

                    THEN("batch is not empty")
                    {
                        REQUIRE(!batch.IsEmpty());
                        REQUIRE(batch.Size() == 1);
                    }

                    THEN("returned matrix contains each created shard")
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

                    THEN("removing fourth shard does not empty batch")
                    {
                        relic->Destroy<Shard<3>>();
                        REQUIRE(!batch.IsEmpty());
                    }
                }
            }
        }
    }
}

SCENARIO_METHOD(AllBatchTestsFixture, "all formed batch", "[all][batch][signal]")
{
    GIVEN("registered reliquary, created relic and created batches")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard<0>>()
            .Register<Shard<1>>()
            .Register<Shard<2>>()
            .Register<Shard<3>>()
            .Actualize();

        auto relic = reliquary->Do<Create<OpenRelic>>();

        auto formedBatch = reliquary->Batch<
            MatrixFormed<All<Either<Shard<0>>, Either<Shard<1>>, Either<Shard<2>>>>>();
        auto dissolvedBatch = reliquary->Batch<
            MatrixDissolved<All<Either<Shard<0>>, Either<Shard<1>>, Either<Shard<2>>>>>();

        WHEN("creating shards")
        {
            auto createdShard0 = relic->Create<Shard<0>>();
            auto createdShard1 = relic->Create<Shard<1>>();
            auto createdShard2 = relic->Create<Shard<2>>();

            THEN("formed batch is not empty")
            {
                REQUIRE(!formedBatch.IsEmpty());
                REQUIRE(formedBatch.Size() == 1);
            }

            THEN("formed batch contains matrix")
            {
                auto& first = *formedBatch.begin();
                REQUIRE(std::get<0>(*first.index) == createdShard0);
                REQUIRE(std::get<1>(*first.index) == createdShard1);
                REQUIRE(std::get<2>(*first.index) == createdShard2);
            }

            THEN("formed batch begin is not end")
            {
                REQUIRE(formedBatch.begin() != formedBatch.end());
            }

            THEN("dissolved batch is empty")
            {
                REQUIRE(dissolvedBatch.IsEmpty());
                REQUIRE(dissolvedBatch.Size() == 0);
            }

            THEN("dissolved batch begin is end")
            {
                REQUIRE(dissolvedBatch.begin() == dissolvedBatch.end());
            }

            WHEN("removing one shard")
            {
                relic->Destroy<Shard<0>>();

                THEN("dissolved batch is not empty")
                {
                    REQUIRE(!dissolvedBatch.IsEmpty());
                    REQUIRE(dissolvedBatch.Size() == 1);
                }

                THEN("dissolved batch contains matrix")
                {
                    REQUIRE(dissolvedBatch.begin()->index.ID() == relic->ID());
                    REQUIRE(dissolvedBatch.begin()->index.Owner() == reliquary.get());
                }

                THEN("dissolved batch begin is not end")
                {
                    REQUIRE(dissolvedBatch.begin() != dissolvedBatch.end());
                }
            }
        }
    }
}

SCENARIO_METHOD(AllBatchTestsFixture, "all batch serialization", "[all][batch][serialization]")
{
    GIVEN("saved reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<Shard<0>>()
            .Register<Shard<1>>()
            .Register<Shard<2>>()
            .Actualize();

        auto savedRelic = savedReliquary->Do<Create<OpenRelic>>();
        savedRelic->Create<Shard<0>>();
        savedRelic->Create<Shard<1>>();
        savedRelic->Create<Shard<2>>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<Shard<0>>()
                .Register<Shard<1>>()
                .Register<Shard<2>>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto batch = loadedReliquary->Batch<All<Shard<0>, Shard<1>, Shard<2>>>();

            THEN("batch is occupied")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
            }
        }
    }
}