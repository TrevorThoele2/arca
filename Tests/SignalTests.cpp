#include <catch.hpp>

#include "SignalTests.h"

SCENARIO_METHOD(SignalTestsFixture, "signal", "[signal]")
{
    GIVEN("reliquary")
    {
        auto reliquary = ReliquaryOrigin().Actualize();

        WHEN("raising signal with one method listening")
        {
            auto foundValue = 0;

            reliquary->On<BasicSignal>([&foundValue](const BasicSignal& signal)
                {
                    foundValue = signal.value;
                });

            const auto emittedValue = dataGeneration.Random<int>(
                TestFramework::Range(1, std::numeric_limits<int>::max()));

            BasicSignal signal{ emittedValue };
            reliquary->Raise(std::move(signal));

            THEN("method was fired")
            {
                REQUIRE(foundValue == emittedValue);
            }
        }

        WHEN("raising signal with ten methods listening")
        {
            std::vector<int> foundValues;

            const auto executionCreator = [&reliquary, &foundValues]()
            {
                reliquary->On<BasicSignal>([&foundValues](const BasicSignal& signal)
                    {
                        foundValues.push_back(signal.value);
                    });
            };

            for(auto i = 0; i < 10; ++i)
                executionCreator();

            const auto emittedValue = dataGeneration.Random<int>(
                TestFramework::Range(1, std::numeric_limits<int>::max()));

            BasicSignal signal{ emittedValue };
            reliquary->Raise(std::move(signal));

            THEN("custom emission logic is fired")
            {
                REQUIRE(foundValues.size() == 10);
                for(auto& value : foundValues)
                {
                    REQUIRE(value == emittedValue);
                }
            }
        }
    }
}