#include <catch.hpp>

#include "SignalTests.h"

SCENARIO_METHOD(SignalTestsFixture, "signal", "[signal]")
{
    GIVEN("reliquary")
    {
        auto reliquary = ReliquaryOrigin().Actualize();

        WHEN("raising signal")
        {
            reliquary->Raise<BasicSignal>(1);

            THEN("reliquary has signal size of 1")
            {
                REQUIRE(reliquary->SignalSize() == 1);
            }
        }

        WHEN("raising signal with custom emission logic")
        {
            int foundValue = 0;

            reliquary->ExecuteOn<BasicSignal>([&foundValue](const BasicSignal& signal)
                {
                    foundValue = signal.value;
                });

            const auto emittedValue = dataGeneration.Random<int>(
                TestFramework::Range(1, std::numeric_limits<int>::max()));

            const BasicSignal signal{ emittedValue };
            reliquary->Raise(signal);

            THEN("custom emission logic is fired")
            {
                REQUIRE(foundValue == emittedValue);
            }
        }

        WHEN("creating second reliquary and transfering all relics to second reliquary")
        {
            auto secondReliquary = ReliquaryOrigin().Actualize();

            reliquary->ExecuteOn<TransferableSignal>(
                [secondReliquary = &*secondReliquary](const TransferableSignal& signal)
                {
                    signal.Raise(*secondReliquary);
                });

            WHEN("raising signal with custom emission logic")
            {
                int foundValue = 0;

                secondReliquary->ExecuteOn<BasicSignal>([&foundValue](const BasicSignal& signal)
                    {
                        foundValue = signal.value;
                    });

                const auto emittedValue = dataGeneration.Random<int>(
                    TestFramework::Range(1, std::numeric_limits<int>::max()));

                const BasicSignal signal{ emittedValue };
                reliquary->Raise(signal);

                THEN("custom emission logic is fired")
                {
                    REQUIRE(foundValue == emittedValue);
                }
            }
        }
    }
}

SCENARIO_METHOD(SignalTestsFixture, "batching transferable signals", "[signal][transferable]")
{
    GIVEN("reliquary and batch of transferable signals")
    {
        auto reliquary = ReliquaryOrigin().Actualize();

        auto batch = reliquary->Batch<TransferableSignal>();

        WHEN("raising signal")
        {
            reliquary->Raise<BasicSignal>(1);

            THEN("batch has size of 1")
            {
                REQUIRE(batch.Size() == 1);
            }

            THEN("batch element has type name of raised signal")
            {
                REQUIRE(batch.begin()->TransferringTypeName() == Traits<BasicSignal>::typeName);
            }

            WHEN("working reliquary")
            {
                reliquary->Work();

                THEN("batch is empty")
                {
                    REQUIRE(batch.IsEmpty());
                }
            }
        }
    }
}

SCENARIO_METHOD(SignalTestsFixture, "batching empty signals", "[signal]")
{
    GIVEN("reliquary and batch of transferable signals")
    {
        auto reliquary = ReliquaryOrigin().Actualize();

        auto batch = reliquary->Batch<EmptySignal>();

        WHEN("raising signal")
        {
            reliquary->Raise<EmptySignal>();

            THEN("batch has size of 1")
            {
                REQUIRE(batch.Size() == 1);
            }
        }
    }
}