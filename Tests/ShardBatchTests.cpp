#include <catch.hpp>

#include "ShardBatchTests.h"

#include <Arca/ExtractShards.h>

ShardBatchFixture::Shard::Shard(int value) :
    value(value)
{}

RelicStructure ShardBatchFixture::StaticRelic::Structure() const
{
    return StructureFrom<Shards>();
}

void ShardBatchFixture::StaticRelic::DoInitialize()
{
    auto tuple = ExtractShards<Shards>(ID(), Owner());
    shard = std::get<0>(tuple);
}

namespace Arca
{
    const TypeHandle ShardTraits<ShardBatchFixture::Shard>::typeHandle = "ShardBatchTestsShard";
    const TypeHandle ShardTraits<ShardBatchFixture::UnregisteredShard>::typeHandle = "ShardBatchTestsUnregisteredShard";
    const TypeHandle RelicTraits<ShardBatchFixture::StaticRelic>::typeHandle = "ShardBatchTestsStaticRelic";
}

SCENARIO_METHOD(ShardBatchFixture, "default shard batch", "[ShardBatch]")
{
    GIVEN("default shard batch")
    {
        ShardBatch<Shard> batch;

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
        auto relic = reliquary.CreateRelic();

        WHEN("starting batch with unregistered shard")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_AS(reliquary.ShardBatch<UnregisteredShard>(), NotRegistered);
            }
        }

        WHEN("creating shard")
        {
            auto createdShard = relic.CreateShard<Shard>();

            WHEN("starting batch")
            {
                auto batch = reliquary.ShardBatch<Shard>();

                THEN("batch contains shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == createdShard);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    relic.DestroyShard<Shard>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary.ShardBatch<Shard>();

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
                auto createdShard = relic.CreateShard<Shard>();
                
                THEN("batch contains derived shard")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned shard is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == createdShard);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing shard empties the batch")
                {
                    relic.DestroyShard<Shard>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }
    }

    GIVEN("registered reliquary with static relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .StaticRelic<StaticRelic>()
            .Actualize();

        WHEN("starting batch")
        {
            auto batch = reliquary.ShardBatch<Shard>();

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
    GIVEN("saved reliquary ")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .Actualize();

        auto savedRelic = savedReliquary.CreateRelic();
        savedRelic.CreateShard<Shard>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Shard<Shard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(loadedReliquary);
            }

            auto batch = loadedReliquary.ShardBatch<Shard>();

            THEN("batch is occupied")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
            }
        }
    }
}