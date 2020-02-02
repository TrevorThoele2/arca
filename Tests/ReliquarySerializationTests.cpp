#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "ReliquarySerializationTests.h"

ReliquarySerializationTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(myValue)
{}

ReliquarySerializationTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

ReliquarySerializationTestsFixture::TypedClosedRelic::TypedClosedRelic(Initialization initialization) :
    ClosedTypedRelic(initialization)
{
    basicShard = FindOrCreate<BasicShard>();
}

ReliquarySerializationTestsFixture::TypedClosedRelic::TypedClosedRelic(Initialization initialization, int myInt) :
    ClosedTypedRelic(initialization), myInt(myInt)
{
    basicShard = Create<BasicShard>();
}

ReliquarySerializationTestsFixture::TypedOpenRelic::TypedOpenRelic(Initialization initialization) :
    OpenTypedRelic(initialization)
{
    basicShard = FindOrCreate<BasicShard>();
}

ReliquarySerializationTestsFixture::TypedOpenRelic::TypedOpenRelic(Initialization initialization, int myInt) :
    OpenTypedRelic(initialization), myInt(myInt)
{
    basicShard = Create<BasicShard>();
}

ReliquarySerializationTestsFixture::GlobalRelic::GlobalRelic(Initialization initialization) :
    ClosedTypedRelic(initialization)
{
    basicShard = FindOrCreate<BasicShard>();
}

ReliquarySerializationTestsFixture::GlobalRelic::GlobalRelic(Initialization initialization, int myInt, std::string shardData)
    : ClosedTypedRelic(initialization), myInt(myInt)
{
    basicShard = Create<BasicShard>(std::move(shardData));
}

ReliquarySerializationTestsFixture::BasicShardNullInscription::BasicShardNullInscription(std::string myValue)
    : myValue(std::move(myValue))
{}

ReliquarySerializationTestsFixture::OtherShardNullInscription::OtherShardNullInscription(int myValue)
    : myValue(myValue)
{}

ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription::TypedClosedRelicNullInscription(
    Initialization initialization)
    :
    ClosedTypedRelic(initialization)
{
    basicShard = FindOrCreate<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription::TypedClosedRelicNullInscription(
    Initialization initialization, int myInt)
    :
    ClosedTypedRelic(initialization), myInt(myInt)
{
    basicShard = Create<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription::TypedOpenRelicNullInscription(
    Initialization initialization)
    :
    OpenTypedRelic(initialization)
{
    basicShard = FindOrCreate<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription::TypedOpenRelicNullInscription(
    Initialization initialization, int myInt)
    :
    OpenTypedRelic(initialization), myInt(myInt)
{
    basicShard = Create<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::GlobalRelicNullInscription::GlobalRelicNullInscription(
    Initialization initialization)
    :
    ClosedTypedRelic(initialization)
{
    basicShard = FindOrCreate<BasicShardNullInscription>();
}

ReliquarySerializationTestsFixture::GlobalRelicNullInscription::GlobalRelicNullInscription(
    Initialization initialization, int myInt, std::string shardData)
    :
    ClosedTypedRelic(initialization), myInt(myInt)
{
    basicShard = Create<BasicShardNullInscription>(std::move(shardData));
}

ReliquarySerializationTestsFixture::MovableOnlyRelic::MovableOnlyRelic(Initialization initialization)
    : ClosedTypedRelic(initialization)
{
    basicShard = FindOrCreate<BasicShard>();
}

ReliquarySerializationTestsFixture::MovableOnlyRelic::MovableOnlyRelic(
    Initialization initialization, int myInt, std::string shardData)
    :
    ClosedTypedRelic(initialization), myValue(myInt)
{
    basicShard = Create<BasicShard>(std::move(shardData));
}

const Inscription::BinaryArchive::StreamPosition defaultOutputArchiveSize = 135;

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

            auto loadedRelic = Arca::RelicIndex<OpenRelic>(savedRelic->ID(), *loadedReliquary);
            auto shardFromRelic = loadedRelic->Find<BasicShard>();

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic->ID()));
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = Arca::ShardIndex<BasicShard>(loadedRelic->ID(), *loadedReliquary);
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

            auto loadedRelic = Arca::RelicIndex<OpenRelic>(savedRelic->ID(), *loadedReliquary);

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic->ID()));
            }

            THEN("relic does not have shard")
            {
                REQUIRE_THROWS_AS(loadedRelic->Find<BasicShard>().Get(), NotRegistered);
                REQUIRE_THROWS_AS(Arca::ShardIndex<BasicShard>(loadedRelic->ID(), *loadedReliquary).Get(), NotRegistered);
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

            auto loadedRelic = Arca::RelicIndex<OpenRelic>(savedRelic->ID(), *loadedReliquary);
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
                    Arca::ShardIndex<BasicShardWithDifferentInputHandle>(loadedRelic->ID(), *loadedReliquary);
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

        auto savedRelic = Arca::GlobalIndex<GlobalRelic>(*savedReliquary);

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

            auto loadedRelic = Arca::GlobalIndex<GlobalRelic>(*loadedReliquary);

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

            auto loadedRelic = Arca::RelicIndex<TypedClosedRelic>(savedRelic->ID(), *loadedReliquary);

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

            auto loadedRelic = Arca::RelicIndex<TypedOpenRelic>(savedRelic->ID(), *loadedReliquary);
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

            auto loadedRelic = Arca::RelicIndex<MovableOnlyRelic>(savedRelic->ID(), *loadedReliquary);

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

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedSignals = loadedReliquary->Batch<BasicSignal>();

            THEN("has no signals")
            {
                REQUIRE(loadedSignals.IsEmpty());
            }

            THEN("input size is default output size")
            {
                REQUIRE(inputArchiveSize == defaultOutputArchiveSize);
            }
        }
    }
}

SCENARIO_METHOD(ReliquarySerializationTestsFixture, "global computation serialization", "[global][computation][serialization]")
{
    GIVEN("saved reliquary with global computation")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<GlobalRelic>(dataGeneration.Random<int>(), dataGeneration.Random<std::string>())
            .Compute<int>(
                [](Reliquary& reliquary)
                {
                    const GlobalIndex<GlobalRelic> backing(reliquary);
                    return backing->myInt;
                })
            .Actualize();

        auto savedComputation = Arca::ComputedIndex<int>(*savedReliquary);

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<BasicShard>()
                .Register<GlobalRelic>()
                .Compute<int>(
                    [](Reliquary& reliquary)
                    {
                        const GlobalIndex<GlobalRelic> backing(reliquary);
                        return backing->myInt;
                    })
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedComputation = Arca::ComputedIndex<int>(*loadedReliquary);

            THEN("loaded relic has saved value")
            {
                REQUIRE(*loadedComputation == *savedComputation);
            }

            THEN("reliquary has global, shard for global, and created signal")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedReliquary->ShardSize() == 1);
                REQUIRE(loadedReliquary->SignalSize() == 2);
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

        auto savedRelic = Arca::GlobalIndex<GlobalRelicNullInscription>(*savedReliquary);

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

            auto loadedRelic = Arca::GlobalIndex<GlobalRelicNullInscription>(*loadedReliquary);

            THEN("loaded relic does not have saved value")
            {
                REQUIRE(loadedRelic->myInt != savedRelic->myInt);
            }

            THEN("reliquary only has global")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedReliquary->ShardSize() == 1);
                REQUIRE(loadedReliquary->SignalSize() == 2);
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

            auto loadedRelic = Arca::RelicIndex<TypedClosedRelicNullInscription>(savedRelic->ID(), *loadedReliquary);

            THEN("loaded relic does not exist")
            {
                REQUIRE(!loadedRelic);
            }

            THEN("reliquary is empty")
            {
                REQUIRE(loadedReliquary->RelicSize() == 0);
                REQUIRE(loadedReliquary->ShardSize() == 0);
                REQUIRE(loadedReliquary->SignalSize() == 0);
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

            auto loadedRelic = Arca::RelicIndex<TypedOpenRelicNullInscription>(savedRelic->ID(), *loadedReliquary);

            THEN("loaded relic does not exist")
            {
                REQUIRE(!loadedRelic);
            }

            THEN("reliquary is empty")
            {
                REQUIRE(loadedReliquary->RelicSize() == 0);
                REQUIRE(loadedReliquary->ShardSize() == 0);
                REQUIRE(loadedReliquary->SignalSize() == 0);
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
                REQUIRE(loadedReliquary->SignalSize() == 0);
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

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedSignals = loadedReliquary->Batch<BasicSignalNullInscription>();

            THEN("has no signals")
            {
                REQUIRE(loadedSignals.IsEmpty());
            }

            THEN("reliquary is empty")
            {
                REQUIRE(loadedReliquary->RelicSize() == 0);
                REQUIRE(loadedReliquary->ShardSize() == 0);
                REQUIRE(loadedReliquary->SignalSize() == 0);
            }

            THEN("input size is default output size")
            {
                REQUIRE(inputArchiveSize == defaultOutputArchiveSize);
            }
        }
    }
}