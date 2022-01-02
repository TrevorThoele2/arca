#include <catch.hpp>
using namespace std::string_literals;

#include "SignaledJsonSerializationTests.h"

#include <Arca/OpenRelic.h>
#include <Arca/Create.h>
#include "SignalListener.h"

SCENARIO_METHOD(
    SignaledJsonSerializationTestsFixture,
    "signaled reliquary json serialization",
    "[reliquary][serialization][json][signal]")
{
    GIVEN("saved empty reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        {
            auto outputArchive = ::Inscription::Archive::OutputJson("Test.dat");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading with open relic with shards")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<BasicShard>()
                .Actualize();

            auto premadeRelic = loadedReliquary->Do(Create<OpenRelic>());
            loadedReliquary->Do(Create<BasicShard>(premadeRelic.ID()));

            auto generalCreated = SignalListener<Created>(*loadedReliquary);
            auto specificRelicCreated = SignalListener<CreatedKnown<OpenRelic>>(*loadedReliquary);
            auto specificShardCreated = SignalListener<CreatedKnown<BasicShard>>(*loadedReliquary);
            auto generalDestroying = SignalListener<Destroying>(*loadedReliquary);
            auto specificRelicDestroying = SignalListener<DestroyingKnown<OpenRelic>>(*loadedReliquary);
            auto specificShardDestroying = SignalListener<DestroyingKnown<BasicShard>>(*loadedReliquary);

            {
                auto inputArchive = ::Inscription::Archive::InputJson(Inscription::File::Path("Test.dat"));
                inputArchive("reliquary", *loadedReliquary);
            }

            THEN("does not signal creation")
            {
                REQUIRE(generalCreated.Executions().empty());
                REQUIRE(specificRelicCreated.Executions().empty());
                REQUIRE(specificShardCreated.Executions().empty());
            }

            THEN("signals destruction")
            {
                REQUIRE(generalDestroying.Executions().size() == 2);
                REQUIRE(specificRelicDestroying.Executions().size() == 1);
                REQUIRE(specificShardDestroying.Executions().size() == 1);
            }
        }
        
        WHEN("loading with typed closed relic with shards")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<LocalRelic>()
                .Register<BasicShard>()
                .Actualize();

            loadedReliquary->Do(Create<LocalRelic>(dataGeneration.Random<int>()));

            auto generalCreated = SignalListener<Created>(*loadedReliquary);
            auto specificRelicCreated = SignalListener<CreatedKnown<LocalRelic>>(*loadedReliquary);
            auto specificShardCreated = SignalListener<CreatedKnown<BasicShard>>(*loadedReliquary);
            auto generalDestroying = SignalListener<Destroying>(*loadedReliquary);
            auto specificRelicDestroying = SignalListener<DestroyingKnown<LocalRelic>>(*loadedReliquary);
            auto specificShardDestroying = SignalListener<DestroyingKnown<BasicShard>>(*loadedReliquary);

            {
                auto inputArchive = ::Inscription::Archive::InputJson(Inscription::File::Path("Test.dat"));
                inputArchive("reliquary", *loadedReliquary);
            }

            THEN("does not signal creation")
            {
                REQUIRE(generalCreated.Executions().empty());
                REQUIRE(specificRelicCreated.Executions().empty());
                REQUIRE(specificShardCreated.Executions().empty());
            }

            THEN("signals destruction")
            {
                REQUIRE(generalDestroying.Executions().size() == 2);
                REQUIRE(specificRelicDestroying.Executions().size() == 1);
                REQUIRE(specificShardDestroying.Executions().size() == 1);
            }
        }
    }

    GIVEN("saved open relic with shards")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<OpenRelic>());
        savedReliquary->Do(Create<BasicShard>(savedRelic.ID()));

        {
            auto outputArchive = ::Inscription::Archive::OutputJson("Test.dat");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<BasicShard>()
                .Actualize();

            auto generalCreated = SignalListener<Created>(*loadedReliquary);
            auto specificRelicCreated = SignalListener<CreatedKnown<OpenRelic>>(*loadedReliquary);
            auto specificShardCreated = SignalListener<CreatedKnown<BasicShard>>(*loadedReliquary);
            auto generalDestroying = SignalListener<Destroying>(*loadedReliquary);
            auto specificRelicDestroying = SignalListener<DestroyingKnown<OpenRelic>>(*loadedReliquary);
            auto specificShardDestroying = SignalListener<DestroyingKnown<BasicShard>>(*loadedReliquary);

            {
                auto inputArchive = ::Inscription::Archive::InputJson(Inscription::File::Path("Test.dat"));
                inputArchive("reliquary", *loadedReliquary);
            }

            THEN("signals creation")
            {
                REQUIRE(generalCreated.Executions().size() == 2);
                REQUIRE(specificRelicCreated.Executions().size() == 1);
                REQUIRE(specificShardCreated.Executions().size() == 1);
            }

            THEN("does not signal destruction")
            {
                REQUIRE(generalDestroying.Executions().empty());
                REQUIRE(specificRelicDestroying.Executions().empty());
                REQUIRE(specificShardDestroying.Executions().empty());
            }
        }
    }
    
    GIVEN("saved closed relic with shards")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<LocalRelic>()
            .Register<BasicShard>()
            .Actualize();

        savedReliquary->Do(Create<LocalRelic>(dataGeneration.Random<int>()));

        {
            auto outputArchive = ::Inscription::Archive::OutputJson("Test.dat");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<LocalRelic>()
                .Register<BasicShard>()
                .Actualize();

            auto generalCreated = SignalListener<Created>(*loadedReliquary);
            auto specificRelicCreated = SignalListener<CreatedKnown<LocalRelic>>(*loadedReliquary);
            auto specificShardCreated = SignalListener<CreatedKnown<BasicShard>>(*loadedReliquary);
            auto generalDestroying = SignalListener<Destroying>(*loadedReliquary);
            auto specificRelicDestroying = SignalListener<DestroyingKnown<LocalRelic>>(*loadedReliquary);
            auto specificShardDestroying = SignalListener<DestroyingKnown<BasicShard>>(*loadedReliquary);

            {
                auto inputArchive = ::Inscription::Archive::InputJson(Inscription::File::Path("Test.dat"));
                inputArchive("reliquary", *loadedReliquary);
            }

            THEN("signals creation")
            {
                REQUIRE(generalCreated.Executions().size() == 2);
                REQUIRE(specificRelicCreated.Executions().size() == 1);
                REQUIRE(specificShardCreated.Executions().size() == 1);
            }

            THEN("does not signal destruction")
            {
                REQUIRE(generalDestroying.Executions().empty());
                REQUIRE(specificRelicDestroying.Executions().empty());
                REQUIRE(specificShardDestroying.Executions().empty());
            }
        }
    }
}