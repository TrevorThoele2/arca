#include <catch.hpp>

#include "SignalBatchTests.h"

SignalBatchFixture::SignalBatchFixture()
{
    auto typeGroup = typeRegistration.CreateGroup();
    typeGroup->RegisterSignal<BasicSignal>();
}

SCENARIO_METHOD(SignalBatchFixture, "SignalBatch")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = CreateRegistered<Reliquary>();

        WHEN("starting batch with unregistered signal")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_AS(reliquary.StartSignalBatch<UnregisteredSignal>(), NotRegistered);
            }
        }

        WHEN("initializing")
        {
            WHEN("raising signal")
            {
                BasicSignal signal{};
                signal.myValue = dataGeneration.Random<int>();
                reliquary.RaiseSignal(signal);

                THEN("starting batch contains raised signal")
                {
                    auto batch = reliquary.StartSignalBatch<BasicSignal>();

                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                    REQUIRE(batch.begin()->myValue == signal.myValue);
                }
            }

            WHEN("starting batch")
            {
                auto batch = reliquary.StartSignalBatch<BasicSignal>();

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
}