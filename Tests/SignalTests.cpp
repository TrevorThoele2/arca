#include <catch.hpp>

#include "SignalTests.h"

namespace Arca
{
    const TypeName Traits<::SignalTestsFixture::BasicSignal>::typeName =
        "SignalTestsBasicSignal";
}

SCENARIO_METHOD(SignalTestsFixture, "signal", "[signal]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicSignal>()
            .Actualize();

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

        WHEN("registering second reliquary and transfering all relics to second reliquary")
        {
            auto secondReliquary = ReliquaryOrigin()
                .Type<BasicSignal>()
                .Actualize();

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