#include <catch.hpp>

#include "SelfContainedCommandTests.h"

#include "SignalListener.h"

SelfContainedCommandTestsFixture::SelfContainedCommand::SelfContainedCommand() : executionCount(std::make_unique<int>())
{}

void SelfContainedCommandTestsFixture::SelfContainedCommand::Do(Arca::Reliquary&) const
{
    ++*executionCount;
}

int SelfContainedCommandTestsFixture::SelfContainedCommandWithResult::Do(Arca::Reliquary&) const
{
    return 123456;
}

SCENARIO_METHOD(SelfContainedCommandTestsFixture, "self contained command", "[command][selfcontained]")
{
    GIVEN("reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin().Actualize();

        WHEN("emitting command")
        {
            auto command = SelfContainedCommand{};
            reliquary->Do(command);

            THEN("handled it")
            {
                REQUIRE(*command.executionCount == 1);
            }
        }

        WHEN("emitting command with result")
        {
            auto command = SelfContainedCommandWithResult{};
            auto result = reliquary->Do(command);

            THEN("handled it")
            {
                REQUIRE(result == 123456);
            }
        }
    }
}