#include <catch.hpp>

#include "ShardBatchTests.h"

ShardBatchFixture::Shard::Shard(int value) :
    value(value)
{}

void ShardBatchFixture::GlobalRelic::InitializeImplementation()
{
    auto shards = ExtractShards();
    shard = std::get<0>(shards);
}

namespace Arca
{
    const TypeHandleName Traits<ShardBatchFixture::Shard>::typeName
        = "ShardBatchTestsShard";
    const TypeHandleName Traits<ShardBatchFixture::UnregisteredShard>::typeName
        = "ShardBatchTestsUnregisteredShard";
    const TypeHandleName Traits<ShardBatchFixture::GlobalRelic>::typeName
        = "ShardBatchTestsGlobalRelic";
}

SCENARIO_METHOD(ShardBatchFixture, "default shard batch", "[ShardBatch]")
{
    GIVEN("default shard batch")
    {
        Batch<Shard> batch;

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

SCENARIO_METHOD(ShardBatchFixture, "shard batch", "[ShardBatch]")
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
                auto batch = reliquary->Batch<Shard>();

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

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<Shard>();

            THEN("batch is empty")
            {
                REQUIRE(batch.IsEmpty());
                REQUIRE(batch.Size() == 0);
            }

            THEN("begin is end")
            {
                REQUIRE(batch.begin() == batch.end());
            }

            WHEN("creating derived shard")
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
        }
    }

    GIVEN("registered reliquary with global relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .GlobalRelic<GlobalRelic>()
            .Actualize();

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<Shard>();

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

SCENARIO_METHOD(ShardBatchFixture, "shard batch serialization", "[ShardBatch][serialization]")
{
    GIVEN("saved reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .Actualize();

        auto savedRelic = savedReliquary->Create<OpenRelic>();
        savedRelic.Create<Shard>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Shard<Shard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto batch = loadedReliquary->Batch<Shard>();

            THEN("batch is occupied")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
            }
        }
    }
}