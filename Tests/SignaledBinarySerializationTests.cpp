#include <catch.hpp>
using namespace std::string_literals;

#include "SignaledBinarySerializationTests.h"

#include <Arca/OpenRelic.h>
#include "SignalListener.h"

SCENARIO_METHOD(
    SignaledBinarySerializationTestsFixture,
    "signaled binary serialization",
    "[reliquary][serialization][binary][signal]")
{
    GIVEN("saved empty reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        {
            auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
            outputArchive(*savedReliquary);
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
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
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
        
        WHEN("loading with closed relic with shards")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
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
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
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

    GIVEN("saved open relic without shards")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<OpenRelic>());
        savedReliquary->Do(Create<BasicShard>(savedRelic.ID()));

        {
            auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
            outputArchive(*savedReliquary);
        }

        WHEN("loading")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Actualize();

            auto generalCreated = SignalListener<Created>(*loadedReliquary);
            auto specificRelicCreated = SignalListener<CreatedKnown<OpenRelic>>(*loadedReliquary);
            auto specificShardCreated = SignalListener<CreatedKnown<BasicShard>>(*loadedReliquary);
            auto generalDestroying = SignalListener<Destroying>(*loadedReliquary);
            auto specificRelicDestroying = SignalListener<DestroyingKnown<OpenRelic>>(*loadedReliquary);
            auto specificShardDestroying = SignalListener<DestroyingKnown<BasicShard>>(*loadedReliquary);

            {
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
            }

            THEN("signals creation")
            {
                REQUIRE(generalCreated.Executions().size() == 1);
                REQUIRE(specificRelicCreated.Executions().size() == 1);
            }

            THEN("does not signal destruction")
            {
                REQUIRE(generalDestroying.Executions().empty());
                REQUIRE(specificRelicDestroying.Executions().empty());
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
            auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
            outputArchive(*savedReliquary);
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
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
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