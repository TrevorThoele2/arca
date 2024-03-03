#include <catch.hpp>

#include "CommandTests.h"

void CommandTestsFixture::Curator::Handle(const Command& command)
{
    handledCommands.push_back(command);
}

int CommandTestsFixture::Curator::Handle(const CommandWithResult& command)
{
    return 5;
}

void CommandTestsFixture::CuratorWithSameLink::Handle(const Command& command)
{}

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

SCENARIO_METHOD(CommandTestsFixture, "command result", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<Curator>()
            .Actualize();

        WHEN("emitting command with result")
        {
            auto result = reliquary->Do<CommandWithResult>();

            THEN("result value is correct")
            {
                REQUIRE(result == 5);
            }
        }
    }
}

SCENARIO_METHOD(CommandTestsFixture, "command linked multiple times", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquaryOrigin = Arca::ReliquaryOrigin()
            .Register<Curator>()
            .Register<CuratorWithSameLink>();

        WHEN("acualizing")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    Arca::CommandAlreadyLinked,
                    Catch::Matchers::Message(
                        "The command (" + Arca::TypeFor<Command>().name + ") has already been linked.")
                );
            }
        }
    }
}