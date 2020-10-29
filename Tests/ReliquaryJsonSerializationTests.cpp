#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryJsonSerializationTests.h"

#include <Arca/LocalRelic.h>

#include "SignalListener.h"

SCENARIO_METHOD(ReliquaryJsonSerializationTestsFixture, "reliquary json serialization", "[reliquary][serialization][json]")
{
    GIVEN("registered reliquary with three open relics created")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Actualize();

        auto relic0 = savedReliquary->Do(Create<OpenRelic>());
        auto relic1 = savedReliquary->Do(Create<OpenRelic>());
        auto relic2 = savedReliquary->Do(Create<OpenRelic>());

        WHEN("saving reliquary")
        {
            {
                auto output = Inscription::OutputJsonArchive("Test.dat");
                output("reliquary", *savedReliquary);
            }

            WHEN("loading reliquary and creating three new open relics")
            {
                const auto loadedReliquary = ReliquaryOrigin()
                    .Register<OpenRelic>()
                    .Actualize();

                auto input = Inscription::InputJsonArchive("Test.dat");
                input("reliquary", *loadedReliquary);

                auto relic3 = loadedReliquary->Do(Create<OpenRelic>());
                auto relic4 = loadedReliquary->Do(Create<OpenRelic>());
                auto relic5 = loadedReliquary->Do(Create<OpenRelic>());

                THEN("all loaded relics have greater IDs than saved")
                {
                    REQUIRE(relic3.ID() > relic0.ID());
                    REQUIRE(relic3.ID() > relic1.ID());
                    REQUIRE(relic3.ID() > relic2.ID());
                    REQUIRE(relic4.ID() > relic0.ID());
                    REQUIRE(relic4.ID() > relic1.ID());
                    REQUIRE(relic4.ID() > relic2.ID());
                    REQUIRE(relic5.ID() > relic0.ID());
                    REQUIRE(relic5.ID() > relic1.ID());
                    REQUIRE(relic5.ID() > relic2.ID());
                }
            }
        }
    }

    GIVEN("saved empty reliquary with every type registered")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<TypedClosedRelic>()
            .Register<TypedOpenRelic>()
            .Register<BasicShard>()
            .Register<GlobalRelic>()
            .Register<BasicCurator>()
            .CuratorConstructionPipeline(Pipeline())
            .Actualize();

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<TypedClosedRelic>()
                .Register<TypedOpenRelic>()
                .Register<BasicShard>()
                .Register<GlobalRelic>()
                .Register<BasicCurator>()
                .CuratorConstructionPipeline(Pipeline())
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            THEN("has only global relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
            }
        }
    }

    GIVEN("saved reliquary with open relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<OpenRelic>());
        auto savedShard = savedReliquary->Do(
            Create<BasicShard>(savedRelic.ID(), dataGeneration.Random<std::string>()));

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<BasicShard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<OpenRelic>(savedRelic.ID(), *loadedReliquary);
            auto shardFromRelic = Arca::Index<BasicShard>(loadedRelic.ID(), *loadedReliquary);

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic.ID()));
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = Arca::Index<BasicShard>(loadedRelic.ID(), *loadedReliquary);
                REQUIRE(shardFromReliquary);
                REQUIRE(shardFromRelic);
            }

            THEN("shard has saved value")
            {
                REQUIRE(shardFromRelic->myValue == savedShard->myValue);
            }

            THEN("relic id is saved id")
            {
                REQUIRE(loadedRelic.ID() == savedRelic.ID());
            }
        }

        WHEN("loading reliquary without registering shard type")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<OpenRelic>(savedRelic.ID(), *loadedReliquary);

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic.ID()));
            }

            THEN("relic does not have shard")
            {
                REQUIRE_THROWS_AS(Arca::Index<BasicShard>(loadedRelic.ID(), *loadedReliquary).Get(), NotRegistered);
            }

            THEN("relic id is saved id")
            {
                REQUIRE(loadedRelic.ID() == savedRelic.ID());
            }
        }

        WHEN("loading reliquary with different shard type with same input type handle")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<BasicShardWithDifferentInputHandle>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<OpenRelic>(savedRelic.ID(), *loadedReliquary);
            auto shardFromRelic = Arca::Index<BasicShardWithDifferentInputHandle>(loadedRelic.ID(), *loadedReliquary);

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic.ID()));
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary =
                    Arca::Index<BasicShardWithDifferentInputHandle>(loadedRelic.ID(), *loadedReliquary);
                REQUIRE(shardFromReliquary);
                REQUIRE(shardFromRelic);
            }

            THEN("shard has saved value")
            {
                REQUIRE(shardFromRelic->myValue == savedShard->myValue);
            }

            THEN("relic id is saved id")
            {
                REQUIRE(loadedRelic.ID() == savedRelic.ID());
            }
        }
    }

    GIVEN("saved reliquary with global relic")
    {
        auto relicData = dataGeneration.Random<int>();
        auto shardData = dataGeneration.Random<std::string>();

        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<GlobalRelic>(relicData, shardData)
            .Actualize();

        auto savedRelic = Arca::Index<GlobalRelic>(*savedReliquary);

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<GlobalRelic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<GlobalRelic>(*loadedReliquary);

            THEN("loaded relic has value of saved")
            {
                REQUIRE(loadedRelic->myInt == savedRelic->myInt);
            }

            THEN("loaded relic has shard of saved")
            {
                REQUIRE(loadedRelic->basicShard->myValue == savedRelic->basicShard->myValue);
            }
        }
    }

    GIVEN("saved reliquary with typed closed relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<TypedClosedRelic>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<TypedClosedRelic>{dataGeneration.Random<int>()});

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<TypedClosedRelic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<TypedClosedRelic>(savedRelic.ID(), *loadedReliquary);

            THEN("loaded relic has value of saved")
            {
                REQUIRE(loadedRelic->myInt == savedRelic->myInt);
            }

            THEN("loaded relic has shard of saved")
            {
                REQUIRE(loadedRelic->basicShard->myValue == savedRelic->basicShard->myValue);
            }
        }
    }

    GIVEN("saved reliquary with typed open relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<OtherShard>()
            .Register<TypedOpenRelic>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<TypedOpenRelic>{dataGeneration.Random<int>()});
        auto savedOtherShard = savedReliquary->Do(Create<OtherShard>(savedRelic.ID()));

        {
            auto outputArchive = Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<OtherShard>()
                .Register<TypedOpenRelic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<TypedOpenRelic>(savedRelic.ID(), *loadedReliquary);
            auto loadedOtherShard = Arca::Index<OtherShard>(loadedRelic.ID(), *loadedReliquary);

            THEN("loaded relic has value of saved")
            {
                REQUIRE(loadedRelic->myInt == savedRelic->myInt);
            }

            THEN("loaded relic has shard of saved")
            {
                REQUIRE(loadedRelic->basicShard->myValue == savedRelic->basicShard->myValue);
            }

            THEN("loaded relic has other shard of saved")
            {
                REQUIRE(savedOtherShard->myValue == loadedOtherShard->myValue);
            }
        }
    }

    GIVEN("saved reliquary with movable only relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<MovableOnlyRelic>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<MovableOnlyRelic> {
            dataGeneration.Random<int>(),
            dataGeneration.Random<std::string>()});

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<MovableOnlyRelic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<MovableOnlyRelic>(savedRelic.ID(), *loadedReliquary);

            THEN("loaded relic has value of saved")
            {
                REQUIRE(loadedRelic->myValue == savedRelic->myValue);
            }

            THEN("loaded relic has shard of saved")
            {
                REQUIRE(loadedRelic->basicShard->myValue == savedRelic->basicShard->myValue);
            }
        }
    }

    GIVEN("saved reliquary with curator")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicCurator>()
            .Actualize();

        auto& savedCurator = savedReliquary->Find<BasicCurator>();
        savedCurator.myInt = dataGeneration.Random<int>();

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicCurator>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto& loadedCurator = loadedReliquary->Find<BasicCurator>();

            THEN("loaded curator has value of saved")
            {
                REQUIRE(loadedCurator.myInt == savedCurator.myInt);
            }
        }
    }

    GIVEN("saved reliquary with signal")
    {
        auto savedReliquary = ReliquaryOrigin().Actualize();

        savedReliquary->Raise(BasicSignal());

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin().Actualize();

            auto loadedSignals = SignalListener<BasicSignal>(*loadedReliquary);

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            THEN("has no signals")
            {
                REQUIRE(loadedSignals.Executions().empty());
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryJsonSerializationTestsFixture, "preferential json serialization", "[preferential][serialization][json]")
{
    GIVEN("saved reliquary with preferential relic and shard")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<PreferentialSerializationConstructorRelic1>()
            .Register<PreferentialSerializationConstructorShard>()
            .Actualize();

        const auto savedRelic = savedReliquary->Do(Arca::Create<PreferentialSerializationConstructorRelic1>());
        const auto openRelic = savedReliquary->Do(Arca::Create<OpenRelic>());
        savedReliquary->Do(Arca::Create<PreferentialSerializationConstructorShard>(openRelic));

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            const auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<PreferentialSerializationConstructorRelic1>()
                .Register<PreferentialSerializationConstructorShard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<PreferentialSerializationConstructorRelic1>(
                savedRelic.ID(), *loadedReliquary);

            auto loadedOpenRelic = Arca::Index<OpenRelic>(
                openRelic.ID(), *loadedReliquary);

            auto loadedShard = Arca::Index<PreferentialSerializationConstructorShard>(
                loadedOpenRelic.ID(), *loadedReliquary);

            THEN("loaded relic used preferential constructor")
            {
                REQUIRE(loadedRelic->calledPreferential);
            }

            THEN("loaded shard used preferential constructor")
            {
                REQUIRE(loadedShard->calledPreferential);
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryJsonSerializationTestsFixture, "null reliquary json serialization", "[reliquary][serialization][json]")
{
    GIVEN("saved reliquary with global relic")
    {
        auto relicData = dataGeneration.Random<int>();
        auto shardData = dataGeneration.Random<std::string>();

        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShardNullInscription>()
            .Register<GlobalRelicNullInscription<BasicShardNullInscription>>(relicData, shardData)
            .Actualize();

        auto savedRelic = Arca::Index<GlobalRelicNullInscription<BasicShardNullInscription>>(*savedReliquary);

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShardNullInscription>()
                .Register<GlobalRelicNullInscription<BasicShardNullInscription>>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<GlobalRelicNullInscription<BasicShardNullInscription>>(*loadedReliquary);

            THEN("loaded relic does not have saved value")
            {
                REQUIRE(loadedRelic->myInt != savedRelic->myInt);
            }

            THEN("reliquary only has global")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedReliquary->ShardSize() == 1);
            }
        }
    }

    GIVEN("saved reliquary with global relic with non null shard")
    {
        auto relicData = dataGeneration.Random<int>();
        auto shardData = dataGeneration.Random<std::string>();

        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<GlobalRelicNullInscription<BasicShard>>(relicData, shardData)
            .Actualize();

        auto savedRelic = Arca::Index<GlobalRelicNullInscription<BasicShard>>(*savedReliquary);

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.dat");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary with object already setup")
        {
            auto setupRelicData = dataGeneration.Random<int>();
            auto setupShardData = dataGeneration.Random<std::string>();

            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<GlobalRelicNullInscription<BasicShard>>(setupRelicData, setupShardData)
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.dat");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<GlobalRelicNullInscription<BasicShard>>(*loadedReliquary);

            THEN("loaded relic does not have saved value")
            {
                REQUIRE(loadedRelic->myInt != savedRelic->myInt);
            }

            THEN("loaded relic has setup data")
            {
                REQUIRE(loadedRelic->myInt == setupRelicData);
            }

            THEN("loaded shard has setup data")
            {
                REQUIRE(loadedRelic->shard->myValue == setupShardData);
            }

            THEN("reliquary only has global")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedReliquary->ShardSize() == 1);
            }
        }
    }

    GIVEN("saved reliquary with typed closed relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShardNullInscription>()
            .Register<TypedClosedRelicNullInscription<BasicShardNullInscription>>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<TypedClosedRelicNullInscription<BasicShardNullInscription>>{});

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShardNullInscription>()
                .Register<TypedClosedRelicNullInscription<BasicShardNullInscription>>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<TypedClosedRelicNullInscription<BasicShardNullInscription>>(
                savedRelic.ID(), *loadedReliquary);

            THEN("loaded relic exists")
            {
                REQUIRE(loadedRelic);
            }

            THEN("reliquary has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedReliquary->ShardSize() == 0);
            }
        }
    }

    GIVEN("saved reliquary with typed open relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShardNullInscription>()
            .Register<OtherShardNullInscription>()
            .Register<TypedOpenRelicNullInscription<BasicShardNullInscription>>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<TypedOpenRelicNullInscription<BasicShardNullInscription>>{});
        savedReliquary->Do(Create<OtherShardNullInscription>(savedRelic.ID(), dataGeneration.Random<int>()));

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShardNullInscription>()
                .Register<OtherShardNullInscription>()
                .Register<TypedOpenRelicNullInscription<BasicShardNullInscription>>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto loadedRelic = Arca::Index<TypedOpenRelicNullInscription<BasicShardNullInscription>>(
                savedRelic.ID(), *loadedReliquary);

            THEN("loaded relic exists")
            {
                REQUIRE(loadedRelic);
            }

            THEN("reliquary has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedReliquary->ShardSize() == 0);
            }
        }
    }

    GIVEN("saved reliquary with curator")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicCuratorNullInscription>()
            .Actualize();

        auto& savedCurator = savedReliquary->Find<BasicCuratorNullInscription>();
        savedCurator.myInt = dataGeneration.Random<int>();

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicCuratorNullInscription>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            auto& loadedCurator = loadedReliquary->Find<BasicCuratorNullInscription>();

            THEN("loaded curator does not have value of saved")
            {
                REQUIRE(loadedCurator.myInt != savedCurator.myInt);
            }

            THEN("reliquary is empty")
            {
                REQUIRE(loadedReliquary->RelicSize() == 0);
                REQUIRE(loadedReliquary->ShardSize() == 0);
            }
        }
    }

    GIVEN("saved reliquary with signal")
    {
        auto savedReliquary = ReliquaryOrigin().Actualize();

        savedReliquary->Raise(BasicSignalNullInscription{});

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin().Actualize();

            auto loadedSignals = SignalListener<BasicSignalNullInscription>(*loadedReliquary);

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            THEN("has no signals")
            {
                REQUIRE(loadedSignals.Executions().empty());
            }

            THEN("reliquary is empty")
            {
                REQUIRE(loadedReliquary->RelicSize() == 0);
                REQUIRE(loadedReliquary->ShardSize() == 0);
            }
        }
    }
}

SCENARIO_METHOD(
    ReliquaryJsonSerializationTestsFixture,
    "null reliquary json serialization with non default constructor",
    "[reliquary][serialization][json]")
{
    GIVEN("saved reliquary with non default shard")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<NonDefaultConstructorRelic>()
            .Actualize();

        const auto savedRelic = savedReliquary->Do(Create<NonDefaultConstructorRelic>{
            dataGeneration.Random<int>() });

        {
            auto outputArchive = ::Inscription::OutputJsonArchive("Test.json");
            outputArchive("reliquary", *savedReliquary);
        }

        WHEN("loading reliquary")
        {
            const auto loadedReliquary = ReliquaryOrigin()
                .Register<NonDefaultConstructorRelic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputJsonArchive("Test.json");
                inputArchive("reliquary", *loadedReliquary);
            }

            const auto loadedRelic = Arca::Index<NonDefaultConstructorRelic>(savedRelic.ID(), *loadedReliquary);

            THEN("has loaded relic")
            {
                REQUIRE(loadedRelic);
            }
        }
    }
}