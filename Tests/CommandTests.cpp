#include <catch.hpp>

#include "CommandTests.h"

#include "SignalListener.h"

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

CommandTestsFixture::Relic::Relic(Init init) : ClosedTypedRelic(init)
{}

CommandTestsFixture::Relic::Relic(Init init, int integer, const std::string& string) :
    ClosedTypedRelic(init), integer(integer), string(string)
{}

CommandTestsFixture::Shard::Shard(int integer, const std::string& string) :
    integer(integer), string(string)
{}

CommandTestsFixture::RelicWithShard::RelicWithShard(Init init, int integer, const std::string& string) :
    ClosedTypedRelic(init), shard(init.Create<Shard>(integer, string))
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

SCENARIO_METHOD(CommandTestsFixture, "relic copy assignment", "[command]")
{
    auto integers = dataGeneration.RandomGroup<int>(2);
    auto strings = dataGeneration.RandomGroup<std::string>(2);

    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<Relic>()
            .Actualize();

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<Relic>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<Relic>>(*reliquary);

            auto created = reliquary->Do<Arca::Create<Relic>>(integers[0], strings[0]);

            reliquary->Do<Arca::AssignCopy<Relic>>(created.ID(), integers[1], strings[1]);

            THEN("data changes")
            {
                REQUIRE(created->integer == integers[1]);
                REQUIRE(created->integer != integers[0]);
                REQUIRE(created->string == strings[1]);
                REQUIRE(created->string != strings[0]);
            }

            THEN("emits signals")
            {
                REQUIRE(assigningKnown.Executions().size() == 1);
                REQUIRE(assignedKnown.Executions().size() == 1);
                REQUIRE(assigningKnown.Executions().begin()->reference.ID() == created->ID());
                REQUIRE(assignedKnown.Executions().begin()->reference.ID() == created->ID());
            }
        }

        WHEN("emitting command without creating relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES(
                    reliquary->Do<Arca::AssignCopy<Relic>>(1, integers[1], strings[1]),
                    Arca::CannotFind,
                    ::Catch::Matchers::Message("The relic (" + Arca::Traits<Relic>::typeName + ") cannot be found."));
            }
        }
    }

    GIVEN("registered relic with shard")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<RelicWithShard>()
            .Register<Shard>()
            .Actualize();

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<RelicWithShard>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<RelicWithShard>>(*reliquary);

            auto created = reliquary->Do<Arca::Create<RelicWithShard>>(integers[0], strings[0]);

            reliquary->Do<Arca::AssignCopy<RelicWithShard>>(created.ID(), integers[1], strings[1]);

            THEN("assigns new shard")
            {
                REQUIRE(created->shard->integer == integers[1]);
                REQUIRE(created->shard->integer != integers[0]);
                REQUIRE(created->shard->string == strings[1]);
                REQUIRE(created->shard->string != strings[0]);
            }

            THEN("emits signals")
            {
                REQUIRE(assigningKnown.Executions().size() == 1);
                REQUIRE(assignedKnown.Executions().size() == 1);
                REQUIRE(assigningKnown.Executions().begin()->reference.ID() == created->ID());
                REQUIRE(assignedKnown.Executions().begin()->reference.ID() == created->ID());
            }
        }
    }
}

SCENARIO_METHOD(CommandTestsFixture, "relic move assignment", "[command]")
{
    auto integers = dataGeneration.RandomGroup<int>(2);
    auto strings = dataGeneration.RandomGroup<std::string>(2);

    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<Relic>()
            .Actualize();

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<Relic>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<Relic>>(*reliquary);

            auto created = reliquary->Do<Arca::Create<Relic>>(integers[0], strings[0]);

            reliquary->Do<Arca::AssignMove<Relic>>(created.ID(), integers[1], strings[1]);

            THEN("data changes")
            {
                REQUIRE(created->integer == integers[1]);
                REQUIRE(created->integer != integers[0]);
                REQUIRE(created->string == strings[1]);
                REQUIRE(created->string != strings[0]);
            }

            THEN("emits signals")
            {
                REQUIRE(assigningKnown.Executions().size() == 1);
                REQUIRE(assignedKnown.Executions().size() == 1);
                REQUIRE(assigningKnown.Executions().begin()->reference.ID() == created->ID());
                REQUIRE(assignedKnown.Executions().begin()->reference.ID() == created->ID());
            }
        }

        WHEN("emitting command without creating relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES(
                    reliquary->Do<Arca::AssignMove<Relic>>(1, integers[1], strings[1]),
                    Arca::CannotFind,
                    ::Catch::Matchers::Message("The relic (" + Arca::Traits<Relic>::typeName + ") cannot be found."));
            }
        }
    }

    GIVEN("registered relic with shard")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<RelicWithShard>()
            .Register<Shard>()
            .Actualize();

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<RelicWithShard>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<RelicWithShard>>(*reliquary);

            auto created = reliquary->Do<Arca::Create<RelicWithShard>>(integers[0], strings[0]);

            reliquary->Do<Arca::AssignMove<RelicWithShard>>(created.ID(), integers[1], strings[1]);

            THEN("assigns new shard")
            {
                REQUIRE(created->shard->integer == integers[1]);
                REQUIRE(created->shard->integer != integers[0]);
                REQUIRE(created->shard->string == strings[1]);
                REQUIRE(created->shard->string != strings[0]);
            }

            THEN("emits signals")
            {
                REQUIRE(assigningKnown.Executions().size() == 1);
                REQUIRE(assignedKnown.Executions().size() == 1);
                REQUIRE(assigningKnown.Executions().begin()->reference.ID() == created->ID());
                REQUIRE(assignedKnown.Executions().begin()->reference.ID() == created->ID());
            }
        }
    }
}

SCENARIO_METHOD(CommandTestsFixture, "shard copy assignment", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<Shard>()
            .Actualize();

        auto integers = dataGeneration.RandomGroup<int>(2);
        auto strings = dataGeneration.RandomGroup<std::string>(2);

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<Shard>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<Shard>>(*reliquary);

            auto created = reliquary->Do<Arca::Create<Arca::OpenRelic>>();
            created->Create<Shard>(integers[0], strings[0]);

            reliquary->Do<Arca::AssignCopy<Shard>>(created.ID(), integers[1], strings[1]);

            auto shard = created->Find<Shard>();

            THEN("data changes")
            {
                REQUIRE(shard->integer == integers[1]);
                REQUIRE(shard->integer != integers[0]);
                REQUIRE(shard->string == strings[1]);
                REQUIRE(shard->string != strings[0]);
            }

            THEN("emits signals")
            {
                REQUIRE(assigningKnown.Executions().size() == 1);
                REQUIRE(assignedKnown.Executions().size() == 1);
                REQUIRE(assigningKnown.Executions().begin()->reference.ID() == created->ID());
                REQUIRE(assignedKnown.Executions().begin()->reference.ID() == created->ID());
            }
        }

        WHEN("emitting command without creating relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES(
                    reliquary->Do<Arca::AssignCopy<Shard>>(1, integers[1], strings[1]),
                    Arca::CannotFind,
                    ::Catch::Matchers::Message("The shard (" + Arca::Traits<Shard>::typeName + ") cannot be found."));
            }
        }
    }
}

SCENARIO_METHOD(CommandTestsFixture, "shard move assignment", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<Shard>()
            .Actualize();

        auto integers = dataGeneration.RandomGroup<int>(2);
        auto strings = dataGeneration.RandomGroup<std::string>(2);

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<Shard>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<Shard>>(*reliquary);

            auto created = reliquary->Do<Arca::Create<Arca::OpenRelic>>();
            created->Create<Shard>(integers[0], strings[0]);

            reliquary->Do<Arca::AssignMove<Shard>>(created.ID(), integers[1], strings[1]);

            auto shard = created->Find<Shard>();

            THEN("data changes")
            {
                REQUIRE(shard->integer == integers[1]);
                REQUIRE(shard->integer != integers[0]);
                REQUIRE(shard->string == strings[1]);
                REQUIRE(shard->string != strings[0]);
            }

            THEN("emits signals")
            {
                REQUIRE(assigningKnown.Executions().size() == 1);
                REQUIRE(assignedKnown.Executions().size() == 1);
                REQUIRE(assigningKnown.Executions().begin()->reference.ID() == created->ID());
                REQUIRE(assignedKnown.Executions().begin()->reference.ID() == created->ID());
            }
        }

        WHEN("emitting command without creating relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES(
                    reliquary->Do<Arca::AssignMove<Shard>>(1, integers[1], strings[1]),
                    Arca::CannotFind,
                    ::Catch::Matchers::Message("The shard (" + Arca::Traits<Shard>::typeName + ") cannot be found."));
            }
        }
    }
}