#include <catch.hpp>

#include "CommandTests.h"

void CommandTestsFixture::Curator::Handle(const Command& command)
{
    handledCommands.push_back(command);
}

SCENARIO_METHOD(CommandTestsFixture, "command", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<Curator>()
            .Actualize();

        auto& curator = reliquary->Find<Curator>();

        WHEN("emitting command")
        {
            reliquary->Do<Command>();

            THEN("handled it")
            {
                REQUIRE(curator.handledCommands.size() == 1);
            }
        }
    }
}