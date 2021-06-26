#include <catch.hpp>

#include "AllBatchTests.h"

#include "DifferentiableShard.h"

#include <Arca/OpenRelic.h>

AllBatchTestsFixture::Relic::Relic(RelicInit init)
{
    init.Create<DifferentiableShard<0>>();
    init.Create<DifferentiableShard<1>>();
    init.Create<DifferentiableShard<2>>();
}

AllBatchTestsFixture::GlobalRelic::GlobalRelic(RelicInit init)
{
    init.Create<DifferentiableShard<0>>();
    init.Create<DifferentiableShard<1>>();
    init.Create<DifferentiableShard<2>>();
}

SCENARIO_METHOD(AllBatchTestsFixture, "default all shards batch", "[all][batch]")
{
    GIVEN("default all batch")
    {
        Batch<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>> batch;

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
            .Register<OpenRelic>()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Register<DifferentiableShard<3>>()
            .Actualize();
        auto relic = reliquary->Do(Create<OpenRelic>{});

        WHEN("creating shards")
        {
            auto createdShard0 = reliquary->Do(Create<DifferentiableShard<0>>(relic));
            auto createdShard1 = reliquary->Do(Create<DifferentiableShard<1>>(relic));
            auto createdShard2 = reliquary->Do(Create<DifferentiableShard<2>>(relic));

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>();

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
                    reliquary->Do(Destroy<DifferentiableShard<0>>(relic.ID()));
                    REQUIRE(batch.IsEmpty());
                }

                WHEN("adding a new shard")
                {
                    reliquary->Do(Create<DifferentiableShard<3>>(relic));

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
                        reliquary->Do(Destroy<DifferentiableShard<3>>(relic.ID()));
                        REQUIRE(!batch.IsEmpty());
                    }
                }
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>();

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
                auto createdShard0 = reliquary->Do(Create<DifferentiableShard<0>>(relic));
                auto createdShard1 = reliquary->Do(Create<DifferentiableShard<1>>(relic));
                auto createdShard2 = reliquary->Do(Create<DifferentiableShard<2>>(relic));

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
                    reliquary->Do(Destroy<DifferentiableShard<0>>(relic.ID()));
                    REQUIRE(batch.IsEmpty());
                }

                WHEN("adding a new shard")
                {
                    reliquary->Do(Create<DifferentiableShard<3>>(relic));

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
                        reliquary->Do(Destroy<DifferentiableShard<3>>(relic.ID()));
                        REQUIRE(!batch.IsEmpty());
                    }
                }
            }
        }
    }

    GIVEN("registered reliquary with global relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Register<GlobalRelic>()
            .Actualize();

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>();

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
            .Register<OpenRelic>()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Register<DifferentiableShard<3>>()
            .Actualize();
        auto relic = reliquary->Do(Create<OpenRelic>{});

        WHEN("creating shards")
        {
            auto createdShard0 = reliquary->Do(Create<DifferentiableShard<0>>(relic));
            auto createdShard1 = reliquary->Do(Create<DifferentiableShard<1>>(relic));
            auto createdShard2 = reliquary->Do(Create<DifferentiableShard<2>>(relic));

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<All<Either<DifferentiableShard<0>>, Either<DifferentiableShard<1>>, Either<DifferentiableShard<2>>>>();

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
                    reliquary->Do(Destroy<DifferentiableShard<0>>(relic.ID()));
                    REQUIRE(batch.IsEmpty());
                }

                WHEN("adding a new relic")
                {
                    reliquary->Do(Create<DifferentiableShard<3>>(relic));

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
                        reliquary->Do(Destroy<DifferentiableShard<3>>(relic.ID()));
                        REQUIRE(!batch.IsEmpty());
                    }
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
            .Register<OpenRelic>()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Actualize();

        const auto savedRelic = savedReliquary->Do(Create<OpenRelic>{});
        savedReliquary->Do(Create<DifferentiableShard<0>>(savedRelic));
        savedReliquary->Do(Create<DifferentiableShard<1>>(savedRelic));
        savedReliquary->Do(Create<DifferentiableShard<2>>(savedRelic));

        {
            auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<DifferentiableShard<0>>()
                .Register<DifferentiableShard<1>>()
                .Register<DifferentiableShard<2>>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
            }

            auto batch = loadedReliquary->Batch<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>();

            THEN("batch is occupied")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
            }
        }
    }
}