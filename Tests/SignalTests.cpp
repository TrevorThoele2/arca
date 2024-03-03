#include <catch.hpp>

#include "SignalTests.h"

SCENARIO_METHOD(SignalTestsFixture, "signal", "[signal]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Signal<BasicSignal>()
            .Actualize();

        WHEN("raising signal with custom emission logic")
        {
            int foundValue = 0;

            reliquary.ExecuteOnSignal<BasicSignal>([&foundValue](const BasicSignal& signal)
                {
                    foundValue = signal.value;
                });

            const auto emittedValue = dataGeneration.Random<int>(
                TestFramework::Range(1, std::numeric_limits<int>::max()));

            const BasicSignal signal{ emittedValue };
            reliquary.RaiseSignal(signal);

            THEN("custom emission logic is fired")
            {
                REQUIRE(foundValue == emittedValue);
            }
        }
    }
}