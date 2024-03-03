#include <catch.hpp>

#include "SignalBatchTests.h"

SCENARIO_METHOD(SignalBatchFixture, "default signal batch", "[SignalBatch]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin().Signal<BasicSignal>().Actualize();

        WHEN("starting batch with unregistered signal")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_AS(reliquary.SignalBatch<UnregisteredSignal>(), NotRegistered);
            }
        }
    }

    GIVEN("default signal batch")
    {
        SignalBatch<BasicSignal> signalBatch;

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
                REQUIRE_THROWS_AS(signalBatch.begin(), BatchNotSetup);
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
                REQUIRE_THROWS_AS(signalBatch.end(), BatchNotSetup);
            }
        }
    }
}

SCENARIO_METHOD(SignalBatchFixture, "signal batch", "[SignalBatch]")
{
    GIVEN("registered and initialized reliquary")
    {
        auto reliquary = ReliquaryOrigin().Signal<BasicSignal>().Actualize();

        WHEN("raising signal")
        {
            BasicSignal signal{};
            signal.myValue = dataGeneration.Random<int>();
            reliquary.RaiseSignal(signal);

            THEN("starting batch contains raised signal")
            {
                auto batch = reliquary.SignalBatch<BasicSignal>();

                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
                REQUIRE(batch.begin()->myValue == signal.myValue);
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary.SignalBatch<BasicSignal>();

            THEN("raising signal causes batch to contain signal")
            {
                BasicSignal signal;
                signal.myValue = dataGeneration.Random<int>();
                reliquary.RaiseSignal(signal);

                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
                REQUIRE(batch.begin()->myValue == signal.myValue);
            }
        }
    }
}