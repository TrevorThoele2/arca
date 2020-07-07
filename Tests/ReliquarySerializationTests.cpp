#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "ReliquarySerializationTests.h"

#include "SignalListener.h"

ReliquarySerializationTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(myValue)
{}

ReliquarySerializationTestsFixture::PreferentialSerializationConstructorShard::PreferentialSerializationConstructorShard(Serialization) :
    calledPreferential(true)
{}

ReliquarySerializationTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

ReliquarySerializationTestsFixture::TypedClosedRelic::TypedClosedRelic(Init init, int myInt) :
    ClosedTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShard>();
}

ReliquarySerializationTestsFixture::TypedClosedRelic::TypedClosedRelic(Init init, Serialization) :
    ClosedTypedRelic(init)
{
    basicShard = Find<BasicShard>();
}

ReliquarySerializationTestsFixture::TypedOpenRelic::TypedOpenRelic(Init init, int myInt) :
    OpenTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShard>();
}

ReliquarySerializationTestsFixture::TypedOpenRelic::TypedOpenRelic(Init init, Serialization) :
    OpenTypedRelic(init)
{
    basicShard = Find<BasicShard>();
}

ReliquarySerializationTestsFixture::GlobalRelic::GlobalRelic(Init init) :
    ClosedTypedRelic(init)
{
    basicShard = Create<BasicShard>();
}

ReliquarySerializationTestsFixture::GlobalRelic::GlobalRelic(Init init, int myInt, std::string shardData)
    : ClosedTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShard>(std::move(shardData));
}

ReliquarySerializationTestsFixture::NonDefaultConstructorRelic::NonDefaultConstructorRelic(
    Init init, int myInt)
    : ClosedTypedRelic(init), myInt(myInt)
{}

ReliquarySerializationTestsFixture::BasicShardNullInscription::BasicShardNullInscription(std::string myValue)
    : myValue(std::move(myValue))
{}

ReliquarySerializationTestsFixture::OtherShardNullInscription::OtherShardNullInscription(int myValue)
    : myValue(myValue)
{}

ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription::TypedClosedRelicNullInscription(
    Init init)
    :
    ClosedTypedRelic(init)
{
    basicShard = Create<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription::TypedClosedRelicNullInscription(
    Init init, int myInt)
    :
    ClosedTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription::TypedOpenRelicNullInscription(
    Init init)
    :
    OpenTypedRelic(init)
{
    basicShard = Create<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription::TypedOpenRelicNullInscription(
    Init init, int myInt)
    :
    OpenTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::GlobalRelicNullInscription::GlobalRelicNullInscription(
    Init init)
    :
    ClosedTypedRelic(init)
{
    basicShard = Create<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::GlobalRelicNullInscription::GlobalRelicNullInscription(
    Init init, int myInt, std::string shardData)
    :
    ClosedTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShardNullInscription>(std::move(shardData));
}

ReliquarySerializationTestsFixture::MovableOnlyRelic::MovableOnlyRelic(
    Init init, int myInt, std::string shardData)
    :
    ClosedTypedRelic(init), myValue(myInt)
{
    basicShard = Create<BasicShard>(std::move(shardData));
}

ReliquarySerializationTestsFixture::MovableOnlyRelic::MovableOnlyRelic(Init init, Serialization) :
    ClosedTypedRelic(init)
{
    basicShard = Find<BasicShard>();
}

const Inscription::BinaryArchive::StreamPosition defaultOutputArchiveSize = 147;

SCENARIO_METHOD(ReliquarySerializationTestsFixture, "reliquary serialization", "[reliquary][serialization]")
{
    GIVEN("saved empty reliquary with every type registered")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<TypedClosedRelic>()
            .Register<TypedOpenRelic>()
            .Register<BasicShard>()
            .Register<GlobalRelic>()
            .Register<BasicCurator>()
            .CuratorPipeline(Pipeline())
            .Actualize();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<TypedClosedRelic>()
                .Register<TypedOpenRelic>()
                .Register<BasicShard>()
                .Register<GlobalRelic>()
                .Register<BasicCurator>()
                .CuratorPipeline(Pipeline())
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            THEN("has only global relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
            }

            THEN("input size is more than default output size")
            {
                REQUIRE(inputArchiveSize > defaultOutputArchiveSize);
            }
        }
    }

    GIVEN("saved reliquary with open relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do<Create<OpenRelic>>();
        auto savedShard = savedRelic->Create<BasicShard>(dataGeneration.Random<std::string>());

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::Index<OpenRelic>(savedRelic->ID(), *loadedReliquary);
            auto shardFromRelic = loadedRelic->Find<BasicShard>();

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic->ID()));
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = Arca::Index<BasicShard>(loadedRelic->ID(), *loadedReliquary);
                REQUIRE(shardFromReliquary);
                REQUIRE(shardFromRelic);
                REQUIRE(loadedRelic->Contains<BasicShard>());
            }

            THEN("shard has saved value")
            {
                REQUIRE(shardFromRelic->myValue == savedShard->myValue);
            }

            THEN("relic owner is loaded reliquary")
            {
                REQUIRE(&loadedRelic->Owner() == loadedReliquary.get());
            }

            THEN("relic id is saved id")
            {
                REQUIRE(loadedRelic->ID() == savedRelic->ID());
            }

            THEN("input size is more than default output size")
            {
                REQUIRE(inputArchiveSize > defaultOutputArchiveSize);
            }
        }

        WHEN("loading reliquary without registering shard type")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = Arca::Index<OpenRelic>(savedRelic->ID(), *loadedReliquary);

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic->ID()));
            }

            THEN("relic does not have shard")
            {
                REQUIRE_THROWS_AS(loadedRelic->Find<BasicShard>().Get(), NotRegistered);
                REQUIRE_THROWS_AS(Arca::Index<BasicShard>(loadedRelic->ID(), *loadedReliquary).Get(), NotRegistered);
            }

            THEN("relic owner is loaded reliquary")
            {
                REQUIRE(&loadedRelic->Owner() == loadedReliquary.get());
            }

            THEN("relic id is saved id")
            {
                REQUIRE(loadedRelic->ID() == savedRelic->ID());
            }
        }

        WHEN("loading reliquary with different shard type with same input type handle")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShardWithDifferentInputHandle>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = Arca::Index<OpenRelic>(savedRelic->ID(), *loadedReliquary);
            auto shardFromRelic = loadedRelic->Find<BasicShardWithDifferentInputHandle>();

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic->ID()));
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary =
                    Arca::Index<BasicShardWithDifferentInputHandle>(loadedRelic->ID(), *loadedReliquary);
                REQUIRE(shardFromReliquary);
                REQUIRE(shardFromRelic);
                REQUIRE(loadedRelic->Contains<BasicShardWithDifferentInputHandle>());
            }

            THEN("shard has saved value")
            {
                REQUIRE(shardFromRelic->myValue == savedShard->myValue);
            }

            THEN("relic owner is loaded reliquary")
            {
                REQUIRE(&loadedRelic->Owner() == loadedReliquary.get());
            }

            THEN("relic id is saved id")
            {
                REQUIRE(loadedRelic->ID() == savedRelic->ID());
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
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<GlobalRelic>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
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

            THEN("input size is more than default output size")
            {
                REQUIRE(inputArchiveSize > defaultOutputArchiveSize);
            }
        }
    }

    GIVEN("saved reliquary with typed closed relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<TypedClosedRelic>()
            .Actualize();

        auto savedRelic = savedReliquary->Do<Create<TypedClosedRelic>>(dataGeneration.Random<int>());

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<TypedClosedRelic>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::Index<TypedClosedRelic>(savedRelic->ID(), *loadedReliquary);

            THEN("loaded relic has value of saved")
            {
                REQUIRE(loadedRelic->myInt == savedRelic->myInt);
            }

            THEN("loaded relic has shard of saved")
            {
                REQUIRE(loadedRelic->basicShard->myValue == savedRelic->basicShard->myValue);
            }

            THEN("input size is more than default output size")
            {
                REQUIRE(inputArchiveSize > defaultOutputArchiveSize);
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

        auto savedRelic = savedReliquary->Do<Create<TypedOpenRelic>>(dataGeneration.Random<int>());
        auto savedOtherShard = savedRelic->Create<OtherShard>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<OtherShard>()
                .Register<TypedOpenRelic>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::Index<TypedOpenRelic>(savedRelic->ID(), *loadedReliquary);
            auto loadedOtherShard = loadedRelic->Find<OtherShard>();

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

            THEN("input size is more than default output size")
            {
                REQUIRE(inputArchiveSize > defaultOutputArchiveSize);
            }
        }
    }

    GIVEN("saved reliquary with movable only relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<MovableOnlyRelic>()
            .Actualize();

        auto savedRelic = savedReliquary->Do<Create<MovableOnlyRelic>>(
            dataGeneration.Random<int>(),
            dataGeneration.Random<std::string>());

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<MovableOnlyRelic>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::Index<MovableOnlyRelic>(savedRelic->ID(), *loadedReliquary);

            THEN("loaded relic has value of saved")
            {
                REQUIRE(loadedRelic->myValue == savedRelic->myValue);
            }

            THEN("loaded relic has shard of saved")
            {
                REQUIRE(loadedRelic->basicShard->myValue == savedRelic->basicShard->myValue);
            }

            THEN("input size is more than default output size")
            {
                REQUIRE(inputArchiveSize > defaultOutputArchiveSize);
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
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicCurator>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto& loadedCurator = loadedReliquary->Find<BasicCurator>();

            THEN("loaded curator has value of saved")
            {
                REQUIRE(loadedCurator.myInt == savedCurator.myInt);
            }

            THEN("input size is more than default output size")
            {
                REQUIRE(inputArchiveSize > defaultOutputArchiveSize);
            }
        }
    }

    GIVEN("saved reliquary with signal")
    {
        auto savedReliquary = ReliquaryOrigin().Actualize();

        savedReliquary->Raise<BasicSignal>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin().Actualize();

            auto loadedSignals = SignalListener<BasicSignal>(*loadedReliquary);

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            THEN("has no signals")
            {
                REQUIRE(loadedSignals.Executions().empty());
            }

            THEN("input size is default output size")
            {
                REQUIRE(inputArchiveSize == defaultOutputArchiveSize);
            }
        }
    }
}

SCENARIO_METHOD(ReliquarySerializationTestsFixture, "preferential serialization", "[preferential][serialization]")
{
    GIVEN("saved reliquary with preferential relic and shard")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Register<PreferentialSerializationConstructorRelic>()
            .Register<PreferentialSerializationConstructorShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Arca::Create<PreferentialSerializationConstructorRelic>());
        const auto openRelic = savedReliquary->Do(Arca::Create<OpenRelic>());
        openRelic->Create<PreferentialSerializationConstructorShard>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<PreferentialSerializationConstructorRelic>()
                .Register<PreferentialSerializationConstructorShard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = Arca::Index<PreferentialSerializationConstructorRelic>(
                savedRelic->ID(), *loadedReliquary);

            auto loadedOpenRelic = Arca::Index<OpenRelic>(
                openRelic->ID(), *loadedReliquary);

            auto loadedShard = loadedOpenRelic->Find<PreferentialSerializationConstructorShard>();

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

SCENARIO_METHOD(ReliquarySerializationTestsFixture, "null reliquary serialization", "[reliquary][serialization]")
{
    GIVEN("saved reliquary with global relic")
    {
        auto relicData = dataGeneration.Random<int>();
        auto shardData = dataGeneration.Random<std::string>();

        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShardNullInscription>()
            .Register<GlobalRelicNullInscription>(relicData, shardData)
            .Actualize();

        auto savedRelic = Arca::Index<GlobalRelicNullInscription>(*savedReliquary);

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShardNullInscription>()
                .Register<GlobalRelicNullInscription>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::Index<GlobalRelicNullInscription>(*loadedReliquary);

            THEN("loaded relic does not have saved value")
            {
                REQUIRE(loadedRelic->myInt != savedRelic->myInt);
            }

            THEN("reliquary only has global")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedReliquary->ShardSize() == 1);
            }

            THEN("input size is default output size")
            {
                REQUIRE(inputArchiveSize == defaultOutputArchiveSize);
            }
        }
    }

    GIVEN("saved reliquary with typed closed relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShardNullInscription>()
            .Register<TypedClosedRelicNullInscription>()
            .Actualize();

        auto savedRelic = savedReliquary->Do<Create<TypedClosedRelicNullInscription>>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShardNullInscription>()
                .Register<TypedClosedRelicNullInscription>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::Index<TypedClosedRelicNullInscription>(savedRelic->ID(), *loadedReliquary);

            THEN("loaded relic does not exist")
            {
                REQUIRE(!loadedRelic);
            }

            THEN("reliquary is empty")
            {
                REQUIRE(loadedReliquary->RelicSize() == 0);
                REQUIRE(loadedReliquary->ShardSize() == 0);
            }

            THEN("input size is default output size")
            {
                REQUIRE(inputArchiveSize == defaultOutputArchiveSize);
            }
        }
    }

    GIVEN("saved reliquary with typed open relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShardNullInscription>()
            .Register<OtherShardNullInscription>()
            .Register<TypedOpenRelicNullInscription>()
            .Actualize();

        auto savedRelic = savedReliquary->Do<Create<TypedOpenRelicNullInscription>>();
        savedRelic->Create<OtherShardNullInscription>(dataGeneration.Random<int>());

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShardNullInscription>()
                .Register<OtherShardNullInscription>()
                .Register<TypedOpenRelicNullInscription>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::Index<TypedOpenRelicNullInscription>(savedRelic->ID(), *loadedReliquary);

            THEN("loaded relic does not exist")
            {
                REQUIRE(!loadedRelic);
            }

            THEN("reliquary is empty")
            {
                REQUIRE(loadedReliquary->RelicSize() == 0);
                REQUIRE(loadedReliquary->ShardSize() == 0);
            }

            THEN("input size is default output size")
            {
                REQUIRE(inputArchiveSize == defaultOutputArchiveSize);
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
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicCuratorNullInscription>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
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

            THEN("input size is default output size")
            {
                REQUIRE(inputArchiveSize == defaultOutputArchiveSize);
            }
        }
    }

    GIVEN("saved reliquary with signal")
    {
        auto savedReliquary = ReliquaryOrigin().Actualize();

        savedReliquary->Raise<BasicSignalNullInscription>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin().Actualize();

            auto loadedSignals = SignalListener<BasicSignalNullInscription>(*loadedReliquary);

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
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

            THEN("input size is default output size")
            {
                REQUIRE(inputArchiveSize == defaultOutputArchiveSize);
            }
        }
    }
}

SCENARIO_METHOD(
    ReliquarySerializationTestsFixture,
    "null reliquary serialization with non default constructor",
    "[reliquary][serialization]")
{
    GIVEN("saved reliquary with non default shard")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<NonDefaultConstructorRelic>()
            .Actualize();

        const auto& savedRelic = *savedReliquary->Do<Create<NonDefaultConstructorRelic>>(
            dataGeneration.Random<int>());

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            const auto loadedReliquary = ReliquaryOrigin()
                .Register<NonDefaultConstructorRelic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            const auto loadedRelic = Arca::Index<NonDefaultConstructorRelic>(savedRelic.ID(), *loadedReliquary);

            THEN("has no loaded relic")
            {
                REQUIRE(!loadedRelic);
            }
        }
    }
}