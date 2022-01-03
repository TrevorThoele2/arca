#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryBinarySerializationTests.h"

#include <Arca/OpenRelic.h>
#include <Arca/Create.h>
#include <Arca/Destroy.h>
#include <Arca/Clear.h>
#include "SignalListener.h"

#include <Inscription/Binary.h>

SCENARIO_METHOD(ReliquaryBinarySerializationTestsFixture, "reliquary binary serialization", "[reliquary][serialization][binary]")
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
            Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

            WHEN("loading reliquary and creating three new open relics")
            {
                const auto loadedReliquary = ReliquaryOrigin()
                    .Register<OpenRelic>()
                    .Actualize();

                Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

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
            .Register<LocalRelic>()
            .Register<BasicShard>()
            .Register<GlobalRelic>()
            .Register<BasicCurator>()
            .CuratorConstructionPipeline(Pipeline())
            .Actualize();

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<LocalRelic>()
                .Register<BasicShard>()
                .Register<GlobalRelic>()
                .Register<BasicCurator>()
                .CuratorConstructionPipeline(Pipeline())
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

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
        auto savedShard = savedReliquary->Do(Create<BasicShard>(
            savedRelic.ID(), dataGeneration.Random<std::string>()));

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<BasicShard>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<OpenRelic>(savedRelic.ID());
            auto shardFromRelic = loadedReliquary->Find<BasicShard>(loadedRelic.ID());

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic.ID()));
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = loadedReliquary->Find<BasicShard>(loadedRelic.ID());
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

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<OpenRelic>(savedRelic.ID());

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic.ID()));
            }

            THEN("relic does not have shard")
            {
                REQUIRE(loadedReliquary->Find<BasicShard>(loadedRelic.ID()).Get() == nullptr);
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

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<OpenRelic>(savedRelic.ID());
            auto shardFromRelic = loadedReliquary->Find<BasicShardWithDifferentInputHandle>(loadedRelic.ID());

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic.ID()));
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = loadedReliquary->Find<BasicShardWithDifferentInputHandle>(loadedRelic.ID());
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

        auto savedRelic = savedReliquary->Find<GlobalRelic>();

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<GlobalRelic>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<GlobalRelic>();

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
            .Register<LocalRelic>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<LocalRelic>{dataGeneration.Random<int>()});

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<LocalRelic>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<LocalRelic>(savedRelic.ID());

            THEN("loaded relic has value of saved")
            {
                REQUIRE(loadedRelic->myInt == savedRelic->myInt);
            }

            THEN("loaded relic has shard of saved")
            {
                REQUIRE(loadedRelic->basicShard->myValue == savedRelic->basicShard->myValue);
            }

            WHEN("destroying shard")
            {
                THEN("throws error")
                {
                    REQUIRE_THROWS_MATCHES(
                        loadedReliquary->Do(Destroy<BasicShard>(loadedRelic.ID())),
                        Arca::CannotDestroy,
                        ::Catch::Matchers::Message("The shard (" + Chroma::ToString(TypeFor<BasicShard>()) + ") cannot be destroyed."));
                }
            }

            WHEN("clearing relics")
            {
                loadedReliquary->Do(Clear(TypeFor<LocalRelic>()));

                THEN("shard is destroyed")
                {
                    auto shard = loadedReliquary->Find<BasicShard>(loadedRelic.ID());
                    REQUIRE(shard == nullptr);
                    REQUIRE(!shard);
                }
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
            dataGeneration.Random<std::string>() });

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<MovableOnlyRelic>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<MovableOnlyRelic>(savedRelic.ID());

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

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicCurator>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

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

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin().Actualize();

            auto loadedSignals = SignalListener<BasicSignal>(*loadedReliquary);

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            THEN("has no signals")
            {
                REQUIRE(loadedSignals.Executions().empty());
            }
        }
    }

    GIVEN("saved reliquary with shard with relic index")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<ShardWithRelicIndex>()
            .Register<OpenRelic>()
            .Actualize();

        auto openRelic1 = savedReliquary->Do(Create<OpenRelic>());
        auto openRelic2 = savedReliquary->Do(Create<OpenRelic>());
        auto shard = savedReliquary->Do(Create<ShardWithRelicIndex>{openRelic1, openRelic2});

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<ShardWithRelicIndex>()
                .Register<OpenRelic>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedShard = loadedReliquary->Find<ShardWithRelicIndex>(shard.ID());

            THEN("shard has occupied relic index")
            {
                REQUIRE(loadedShard);
                REQUIRE(loadedShard->openRelic);
                REQUIRE(loadedShard->openRelic.ID() == openRelic2.ID());
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryBinarySerializationTestsFixture, "preferential binary serialization", "[preferential][serialization][binary]")
{
    GIVEN("saved reliquary with first preferential relic and shard")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<PreferentialSerializationConstructorRelic1>()
            .Register<PreferentialSerializationConstructorShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Arca::Create<PreferentialSerializationConstructorRelic1>());
        const auto openRelic = savedReliquary->Do(Arca::Create<OpenRelic>());
        savedReliquary->Do(Create<PreferentialSerializationConstructorShard>(openRelic.ID()));

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<PreferentialSerializationConstructorRelic1>()
                .Register<PreferentialSerializationConstructorShard>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<PreferentialSerializationConstructorRelic1>(savedRelic.ID());

            auto loadedOpenRelic = loadedReliquary->Find<OpenRelic>(openRelic.ID());

            auto loadedShard = loadedReliquary->Find<PreferentialSerializationConstructorShard>(loadedOpenRelic.ID());

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

    GIVEN("saved reliquary with second preferential relic and shard")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<PreferentialSerializationConstructorRelic2>()
            .Register<PreferentialSerializationConstructorShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Arca::Create<PreferentialSerializationConstructorRelic2>());
        const auto openRelic = savedReliquary->Do(Arca::Create<OpenRelic>());
        savedReliquary->Do(Create<PreferentialSerializationConstructorShard>(openRelic.ID()));

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<PreferentialSerializationConstructorRelic2>()
                .Register<PreferentialSerializationConstructorShard>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<PreferentialSerializationConstructorRelic2>(savedRelic.ID());

            auto loadedOpenRelic = loadedReliquary->Find<OpenRelic>(openRelic.ID());

            auto loadedShard = loadedReliquary->Find<PreferentialSerializationConstructorShard>(loadedOpenRelic.ID());

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

    GIVEN("saved reliquary with second preferential relic and shard")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<PreferentialSerializationConstructorRelic3>()
            .Register<PreferentialSerializationConstructorShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Arca::Create<PreferentialSerializationConstructorRelic3>());
        const auto openRelic = savedReliquary->Do(Arca::Create<OpenRelic>());
        savedReliquary->Do(Create<PreferentialSerializationConstructorShard>(openRelic.ID()));

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<PreferentialSerializationConstructorRelic3>()
                .Register<PreferentialSerializationConstructorShard>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<PreferentialSerializationConstructorRelic3>(savedRelic.ID());

            auto loadedOpenRelic = loadedReliquary->Find<OpenRelic>(openRelic.ID());

            auto loadedShard = loadedReliquary->Find<PreferentialSerializationConstructorShard>(loadedOpenRelic.ID());

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

    GIVEN("saved reliquary with second preferential relic and shard")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<PreferentialSerializationConstructorRelic4>()
            .Register<PreferentialSerializationConstructorShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Arca::Create<PreferentialSerializationConstructorRelic4>());
        const auto openRelic = savedReliquary->Do(Arca::Create<OpenRelic>());
        savedReliquary->Do(Create<PreferentialSerializationConstructorShard>(openRelic.ID()));

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<PreferentialSerializationConstructorRelic4>()
                .Register<PreferentialSerializationConstructorShard>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<PreferentialSerializationConstructorRelic4>(savedRelic.ID());

            auto loadedOpenRelic = loadedReliquary->Find<OpenRelic>(openRelic.ID());

            auto loadedShard = loadedReliquary->Find<PreferentialSerializationConstructorShard>(loadedOpenRelic.ID());

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

SCENARIO_METHOD(ReliquaryBinarySerializationTestsFixture, "null reliquary binary serialization", "[reliquary][serialization][binary]")
{
    GIVEN("saved reliquary with global relic")
    {
        auto relicData = dataGeneration.Random<int>();
        auto shardData = dataGeneration.Random<std::string>();

        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShardNullInscription>()
            .Register<GlobalRelicNullInscription<BasicShardNullInscription>>(relicData)
            .Actualize();

        auto savedRelic = savedReliquary->Find<GlobalRelicNullInscription<BasicShardNullInscription>>();

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShardNullInscription>()
                .Register<GlobalRelicNullInscription<BasicShardNullInscription>>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<GlobalRelicNullInscription<BasicShardNullInscription>>();

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

        auto savedRelic = savedReliquary->Find<GlobalRelicNullInscription<BasicShard>>();

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary with object already setup")
        {
            auto setupRelicData = dataGeneration.Random<int>();
            auto setupShardData = dataGeneration.Random<std::string>();

            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<GlobalRelicNullInscription<BasicShard>>(setupRelicData, setupShardData)
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<GlobalRelicNullInscription<BasicShard>>();

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
            .Register<LocalRelicNullInscription<BasicShardNullInscription>>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<LocalRelicNullInscription<BasicShardNullInscription>>());

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShardNullInscription>()
                .Register<LocalRelicNullInscription<BasicShardNullInscription>>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto loadedRelic = loadedReliquary->Find<LocalRelicNullInscription<BasicShardNullInscription>>(savedRelic.ID());

            THEN("loaded relic exists")
            {
                REQUIRE(loadedRelic);
            }

            THEN("reliquary has shard")
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

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicCuratorNullInscription>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

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

        savedReliquary->Raise(BasicSignalNullInscription());

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin().Actualize();

            auto loadedSignals = SignalListener<BasicSignalNullInscription>(*loadedReliquary);

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

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
    ReliquaryBinarySerializationTestsFixture,
    "null reliquary binary serialization with non default constructor",
    "[reliquary][serialization][binary]")
{
    GIVEN("saved reliquary with non default shard")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<NonDefaultConstructorRelic>()
            .Actualize();

        const auto savedRelic = savedReliquary->Do(Create<NonDefaultConstructorRelic>(
            dataGeneration.Random<int>()));

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            const auto loadedReliquary = ReliquaryOrigin()
                .Register<NonDefaultConstructorRelic>()
                .Actualize();
            
            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            const auto loadedRelic = loadedReliquary->Find<NonDefaultConstructorRelic>(savedRelic.ID());

            THEN("has loaded relic")
            {
                REQUIRE(loadedRelic);
            }
        }
    }
}