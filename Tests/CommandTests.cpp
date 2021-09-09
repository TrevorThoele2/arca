#include <catch.hpp>

#include "CommandTests.h"

#include <Arca/OpenRelic.h>
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
{
    handledCommands.push_back(command);
}

int CommandTestsFixture::CuratorWithSameResultLink::Handle(const CommandWithResult& command)
{
    return 10;
}

void CommandTestsFixture::ThrowingCurator::Handle(const Command& command)
{
    throw std::runtime_error("Error.");
}

CommandTestsFixture::Relic::Relic(int integer, const std::string& string) :
    integer(integer), string(string)
{}

CommandTestsFixture::RelicWithShard::RelicWithShard(
    Arca::RelicInit init, int integer, const std::string& string)
    :
    shard(init.Create<BasicShard>(integer, string))
{}

CommandTestsFixture::RelicWithShard::RelicWithShard(Arca::RelicInit init, Arca::Serialization)
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
            reliquary->Do(Command());

            THEN("handled it")
            {
                REQUIRE(curator.handledCommands.size() == 1);
            }
        }
    }
}

SCENARIO_METHOD(CommandTestsFixture, "command that throws", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<ThrowingCurator>()
            .Actualize();

        WHEN("emitting command")
        {
            THEN("error propagates")
            {
                REQUIRE_THROWS_AS(reliquary->Do(Command()), std::runtime_error);
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
            auto result = reliquary->Do(CommandWithResult());

            THEN("result value is correct")
            {
                REQUIRE(result == 5);
            }
        }
    }
}

SCENARIO_METHOD(CommandTestsFixture, "void result command linked multiple times", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<Curator>()
            .Register<CuratorWithSameLink>()
            .Actualize();

        auto& curator = reliquary->Find<Curator>();
        auto& otherCurator = reliquary->Find<CuratorWithSameLink>();

        WHEN("emitting command")
        {
            reliquary->Do(Command{});

            THEN("handled it from multiple curators")
            {
                REQUIRE(curator.handledCommands.size() == 1);
                REQUIRE(otherCurator.handledCommands.size() == 1);
            }
        }
    }
}

SCENARIO_METHOD(CommandTestsFixture, "non-void result command linked multiple times", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquaryOrigin = Arca::ReliquaryOrigin()
            .Register<Curator>()
            .Register<CuratorWithSameResultLink>();

        WHEN("acualizing")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Actualize(),
                    Arca::CommandWithReturnValueAlreadyLinked,
                    Catch::Matchers::Message(
                        "The command (" + Arca::TypeFor<CommandWithResult>().name + ") has already been linked and returns a value. Multiple handlers are not allowed when the command has a return value.")
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

            auto created = reliquary->Do(Arca::Create<Relic>{integers[0], strings[0]});

            reliquary->Do(Arca::AssignCopy<Relic>{created.ID(), integers[1], strings[1]});

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
                REQUIRE(assigningKnown.Executions().begin()->index.ID() == created.ID());
                REQUIRE(assignedKnown.Executions().begin()->index.ID() == created.ID());
            }
        }

        WHEN("emitting command without creating relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES(
                    reliquary->Do(Arca::AssignCopy < Relic>{1, integers[1], strings[1]}),
                    Arca::CannotFind,
                    ::Catch::Matchers::Message("The relic (" + Arca::Traits<Relic>::TypeName() + ") cannot be found."));
            }
        }
    }

    GIVEN("registered relic with shard")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<RelicWithShard>()
            .Register<BasicShard>()
            .Actualize();

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<RelicWithShard>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<RelicWithShard>>(*reliquary);

            auto created = reliquary->Do(Arca::Create<RelicWithShard>{integers[0], strings[0]});

            reliquary->Do(Arca::AssignCopy<RelicWithShard>{created.ID(), integers[1], strings[1]});

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
                REQUIRE(assigningKnown.Executions().begin()->index.ID() == created.ID());
                REQUIRE(assignedKnown.Executions().begin()->index.ID() == created.ID());
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

            auto created = reliquary->Do(Arca::Create<Relic>{integers[0], strings[0]});

            reliquary->Do(Arca::AssignMove<Relic>{created.ID(), integers[1], strings[1]});

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
                REQUIRE(assigningKnown.Executions().begin()->index.ID() == created.ID());
                REQUIRE(assignedKnown.Executions().begin()->index.ID() == created.ID());
            }
        }

        WHEN("emitting command without creating relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES(
                    reliquary->Do(Arca::AssignMove<Relic>{1, integers[1], strings[1]}),
                    Arca::CannotFind,
                    ::Catch::Matchers::Message("The relic (" + Arca::Traits<Relic>::TypeName() + ") cannot be found."));
            }
        }
    }

    GIVEN("registered relic with shard")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<RelicWithShard>()
            .Register<BasicShard>()
            .Actualize();

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<RelicWithShard>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<RelicWithShard>>(*reliquary);

            auto created = reliquary->Do(Arca::Create<RelicWithShard>{integers[0], strings[0]});

            reliquary->Do(Arca::AssignMove<RelicWithShard>{created.ID(), integers[1], strings[1]});

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
                REQUIRE(assigningKnown.Executions().begin()->index.ID() == created.ID());
                REQUIRE(assignedKnown.Executions().begin()->index.ID() == created.ID());
            }
        }
    }
}

SCENARIO_METHOD(CommandTestsFixture, "shard copy assignment", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<Arca::OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto integers = dataGeneration.RandomGroup<int>(2);
        auto strings = dataGeneration.RandomGroup<std::string>(2);

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<BasicShard>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<BasicShard>>(*reliquary);

            auto created = reliquary->Do(Arca::Create<Arca::OpenRelic>{});
            reliquary->Do(Arca::Create<BasicShard>(created.ID(), integers[0], strings[0]));

            reliquary->Do(Arca::AssignCopy<BasicShard>{created.ID(), integers[1], strings[1]});

            auto shard = reliquary->Find<BasicShard>(created.ID());

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
                REQUIRE(assigningKnown.Executions().begin()->index.ID() == created.ID());
                REQUIRE(assignedKnown.Executions().begin()->index.ID() == created.ID());
            }
        }

        WHEN("emitting command without creating relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES(
                    reliquary->Do(Arca::AssignCopy<BasicShard>{1, integers[1], strings[1]}),
                    Arca::CannotFind,
                    ::Catch::Matchers::Message("The shard (" + Arca::Traits<BasicShard>::TypeName() + ") cannot be found."));
            }
        }
    }
}

SCENARIO_METHOD(CommandTestsFixture, "shard move assignment", "[command]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = Arca::ReliquaryOrigin()
            .Register<Arca::OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto integers = dataGeneration.RandomGroup<int>(2);
        auto strings = dataGeneration.RandomGroup<std::string>(2);

        WHEN("creating relic and emitting command")
        {
            auto assigningKnown = SignalListener<Arca::AssigningKnown<BasicShard>>(*reliquary);
            auto assignedKnown = SignalListener<Arca::AssignedKnown<BasicShard>>(*reliquary);

            auto created = reliquary->Do(Arca::Create<Arca::OpenRelic>{});
            reliquary->Do(Arca::Create<BasicShard>(created.ID(), integers[0], strings[0]));

            reliquary->Do(Arca::AssignMove<BasicShard>{created.ID(), integers[1], strings[1]});

            auto shard = reliquary->Find<BasicShard>(created.ID());

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
                REQUIRE(assigningKnown.Executions().begin()->index.ID() == created.ID());
                REQUIRE(assignedKnown.Executions().begin()->index.ID() == created.ID());
            }
        }

        WHEN("emitting command without creating relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES(
                    reliquary->Do(Arca::AssignMove < BasicShard>{1, integers[1], strings[1]}),
                    Arca::CannotFind,
                    ::Catch::Matchers::Message("The shard (" + Arca::Traits<BasicShard>::TypeName() + ") cannot be found."));
            }
        }
    }
}