#include <catch.hpp>

#include "RelicBatchTests.h"

RelicBatchFixture::Relic::Relic(const ::Inscription::BinaryTableData<Relic>& data) :
    TypedRelicAutomation(data.base), value(data.value)
{}

void RelicBatchFixture::Relic::InitializeImplementation()
{}

RelicBatchFixture::GlobalRelic::GlobalRelic(const ::Inscription::BinaryTableData<Relic>& data) :
    TypedRelicAutomation(data.base), value(data.value)
{}

void RelicBatchFixture::GlobalRelic::InitializeImplementation()
{}

RelicBatchFixture::Shard::Shard(int value) :
    value(value)
{}

namespace Arca
{
    const TypeHandleName Traits<RelicBatchFixture::Relic>::typeName = "RelicBatchTestsRelic";
    const TypeHandleName Traits<RelicBatchFixture::GlobalRelic>::typeName = "RelicBatchTestsGlobalRelic";
    const TypeHandleName Traits<RelicBatchFixture::UnregisteredRelic>::typeName = "RelicBatchTestsUnregisteredRelic";
    const TypeHandleName Traits<RelicBatchFixture::Shard>::typeName = "RelicBatchTestsShard";
}

SCENARIO_METHOD(RelicBatchFixture, "default relic batch", "[RelicBatch]")
{
    GIVEN("default relic batch")
    {
        Batch<Relic> batch;

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

SCENARIO_METHOD(RelicBatchFixture, "relic batch", "[RelicBatch]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<Relic>()
            .Actualize();

        WHEN("creating relic")
        {
            auto createdRelic = reliquary->Create<Relic>();

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<Relic>();

                THEN("batch contains relic")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("destroying relic empties the batch")
                {
                    reliquary->Destroy(createdRelic);
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<Relic>();

            THEN("batch is empty")
            {
                REQUIRE(batch.IsEmpty());
                REQUIRE(batch.Size() == 0);
            }

            THEN("begin is end")
            {
                REQUIRE(batch.begin() == batch.end());
            }

            WHEN("creating relic")
            {
                auto createdRelic = reliquary->Create<Relic>();

                THEN("batch contains relic")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("destroying relic empties the batch")
                {
                    reliquary->Destroy(*createdRelic);
                    REQUIRE(batch.IsEmpty());
                }
            }
        }
    }
}

SCENARIO_METHOD(RelicBatchFixture, "relic batch serialization", "[RelicBatch][serialization]")
{
    GIVEN("saved reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Relic<Relic>()
            .Actualize();

        savedReliquary->Create<Relic>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Relic<Relic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto batch = loadedReliquary->Batch<Relic>();

            THEN("batch is occupied")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
            }
        }
    }
}