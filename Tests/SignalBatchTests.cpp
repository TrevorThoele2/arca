#include <catch.hpp>

#include "SignalBatchTests.h"

SCENARIO_METHOD(SignalBatchFixture, "default signal batch", "[SignalBatch]")
{
    GIVEN("default signal batch")
    {
        Batch<BasicSignal> signalBatch;

        WHEN("querying size")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(signalBatch.Size(), BatchNotSetup);
            }
        }

        WHEN("querying empty")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(signalBatch.IsEmpty(), BatchNotSetup);
            }
        }

        WHEN("begin")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(signalBatch.begin(), BatchNotSetup);
            }
        }

        WHEN("begin const")
        {
            const auto& constSignalBatch = signalBatch;

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(constSignalBatch.begin(), BatchNotSetup);
            }
        }

        WHEN("end")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(signalBatch.end(), BatchNotSetup);
            }
        }

        WHEN("end const")
        {
            const auto& constSignalBatch = signalBatch;

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(constSignalBatch.end(), BatchNotSetup);
            }
        }
    }
}

SCENARIO_METHOD(SignalBatchFixture, "signal batch", "[SignalBatch]")
{
    GIVEN("registered and initialized reliquary")
    {
        auto reliquary = ReliquaryOrigin().Type<BasicSignal>().Actualize();

        WHEN("raising signal")
        {
            BasicSignal signal{};
            signal.myValue = dataGeneration.Random<int>();
            reliquary->Raise(signal);

            THEN("starting batch contains raised signal")
            {
                auto batch = reliquary->Batch<BasicSignal>();

                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
                REQUIRE(batch.begin()->myValue == signal.myValue);
            }

            THEN("starting transferable batch contains raised signal")
            {
                auto batch = reliquary->Batch<TransferableSignal>();

                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<BasicSignal>();

            THEN("raising signal causes batch to contain signal")
            {
                BasicSignal signal;
                signal.myValue = dataGeneration.Random<int>();
                reliquary->Raise(signal);

                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
                REQUIRE(batch.begin()->myValue == signal.myValue);
            }
        }

        WHEN("starting transferable batch")
        {
            auto batch = reliquary->Batch<TransferableSignal>();

            THEN("raising signal causes batch to contain signal")
            {
                BasicSignal signal;
                signal.myValue = dataGeneration.Random<int>();
                reliquary->Raise(signal);

                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
            }
        }
    }
}