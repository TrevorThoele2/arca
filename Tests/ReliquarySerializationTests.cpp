#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "ReliquarySerializationTests.h"

ReliquarySerializationTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

ReliquarySerializationTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

void ReliquarySerializationTestsFixture::TypedClosedRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void ReliquarySerializationTestsFixture::TypedOpenRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void ReliquarySerializationTestsFixture::GlobalRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void ReliquarySerializationTestsFixture::GlobalRelic::Initialize(int myInt)
{
    this->myInt = myInt;
}

ReliquarySerializationTestsFixture::BasicShardNullInscription::BasicShardNullInscription(std::string myValue) :
    myValue(std::move(myValue))
{}

ReliquarySerializationTestsFixture::OtherShardNullInscription::OtherShardNullInscription(int myValue) :
    myValue(myValue)
{}

void ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void ReliquarySerializationTestsFixture::GlobalRelicNullInscription::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void ReliquarySerializationTestsFixture::MovableOnlyRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

namespace Arca
{
    const TypeName Traits<::ReliquarySerializationTestsFixture::BasicShard>::typeName =
        "ReliquarySerializationTestsBasicShard";

    const TypeName Traits<::ReliquarySerializationTestsFixture::BasicShardWithDifferentInputHandle>::typeName =
        "ReliquarySerializationTestsBasicShardWithDifferentInputHandle";

    const TypeName Traits<::ReliquarySerializationTestsFixture::OtherShard>::typeName =
        "ReliquarySerializationTestsOtherShard";

    const TypeName Traits<::ReliquarySerializationTestsFixture::TypedClosedRelic>::typeName =
        "ReliquarySerializationTestsTypedClosedRelic";

    const TypeName Traits<::ReliquarySerializationTestsFixture::TypedOpenRelic>::typeName =
        "ReliquarySerializationTestsTypedOpenRelic";

    const TypeName Traits<::ReliquarySerializationTestsFixture::GlobalRelic>::typeName =
        "ReliquarySerializationTestsGlobalRelic";

    const TypeName Traits<::ReliquarySerializationTestsFixture::BasicCurator>::typeName =
        "ReliquarySerializationTestsBasicCurator";

    const TypeName Traits<::ReliquarySerializationTestsFixture::BasicSignal>::typeName =
        "ReliquarySerializationTestsBasicSignal";

    const TypeName Traits<::ReliquarySerializationTestsFixture::BasicShardNullInscription>::typeName =
        "ReliquarySerializationTestsBasicShardNullInscription";

    const TypeName Traits<::ReliquarySerializationTestsFixture::OtherShardNullInscription>::typeName =
        "ReliquarySerializationTestsOtherShardNullInscription";

    const TypeName Traits<::ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription>::typeName =
        "ReliquarySerializationTestsTypedClosedRelicNullInscription";

    const TypeName Traits<::ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription>::typeName =
        "ReliquarySerializationTestsTypedOpenRelicNullInscription";

    const TypeName Traits<::ReliquarySerializationTestsFixture::GlobalRelicNullInscription>::typeName =
        "ReliquarySerializationTestsGlobalRelicNullInscription";

    const TypeName Traits<::ReliquarySerializationTestsFixture::MovableOnlyRelic>::typeName =
        "ReliquarySerializationTestsMovableOnlyRelic";

    const TypeName Traits<::ReliquarySerializationTestsFixture::BasicCuratorNullInscription>::typeName =
        "ReliquarySerializationTestsBasicCuratorNullInscription";

    const TypeName Traits<::ReliquarySerializationTestsFixture::BasicSignalNullInscription>::typeName =
        "ReliquarySerializationTestsBasicSignalNullInscription";
}

const Inscription::BinaryArchive::StreamPosition defaultOutputArchiveSize = 127;

SCENARIO_METHOD(ReliquarySerializationTestsFixture, "reliquary serialization", "[reliquary][serialization]")
{
    GIVEN("saved empty reliquary with every type registered")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Type<TypedClosedRelic>()
            .Type<TypedOpenRelic>()
            .Type<BasicShard>()
            .Type<GlobalRelic>()
            .Type<BasicCurator>()
            .CuratorPipeline(Pipeline())
            .Type<BasicSignal>()
            .Actualize();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<TypedClosedRelic>()
                .Type<TypedOpenRelic>()
                .Type<BasicShard>()
                .Type<GlobalRelic>()
                .Type<BasicCurator>()
                .CuratorPipeline(Pipeline())
                .Type<BasicSignal>()
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
            .Type<BasicShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Create<OpenRelic>();
        auto savedShard = savedRelic->Create<BasicShard>();
        savedShard->myValue = dataGeneration.Random<std::string>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicShard>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::LocalPtr<OpenRelic>(savedRelic->ID(), *loadedReliquary);
            auto shardFromRelic = loadedRelic->Find<BasicShard>();

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic->ID()));
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = Arca::LocalPtr<BasicShard>(loadedRelic->ID(), *loadedReliquary);
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

            auto loadedRelic = Arca::LocalPtr<OpenRelic>(savedRelic->ID(), *loadedReliquary);

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
                REQUIRE(loadedReliquary->Contains<OpenRelic>(loadedRelic->ID()));
            }

            THEN("relic does not have shard")
            {
                REQUIRE_THROWS_AS(loadedRelic->Find<BasicShard>(), NotRegistered);
                REQUIRE_THROWS_AS(Arca::LocalPtr<BasicShard>(loadedRelic->ID(), *loadedReliquary), NotRegistered);
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
                .Type<BasicShardWithDifferentInputHandle>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = Arca::LocalPtr<OpenRelic>(savedRelic->ID(), *loadedReliquary);
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
                    Arca::LocalPtr<BasicShardWithDifferentInputHandle>(loadedRelic->ID(), *loadedReliquary);
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
        auto savedReliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Type<GlobalRelic>()
            .Actualize();

        auto savedRelic = Arca::GlobalPtr<GlobalRelic>(*savedReliquary);
        savedRelic->myInt = dataGeneration.Random<int>();
        savedRelic->basicShard->myValue = dataGeneration.Random<std::string>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicShard>()
                .Type<GlobalRelic>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::GlobalPtr<GlobalRelic>(*loadedReliquary);

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
            .Type<BasicShard>()
            .Type<TypedClosedRelic>()
            .Actualize();

        auto savedRelic = savedReliquary->Create<TypedClosedRelic>();
        savedRelic->myInt = dataGeneration.Random<int>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicShard>()
                .Type<TypedClosedRelic>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::LocalPtr<TypedClosedRelic>(savedRelic->ID(), *loadedReliquary);

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
            .Type<BasicShard>()
            .Type<OtherShard>()
            .Type<TypedOpenRelic>()
            .Actualize();

        auto savedRelic = savedReliquary->Create<TypedOpenRelic>();
        auto savedOtherShard = savedRelic->Create<OtherShard>();
        savedOtherShard->myValue = dataGeneration.Random<int>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicShard>()
                .Type<OtherShard>()
                .Type<TypedOpenRelic>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::LocalPtr<TypedOpenRelic>(savedRelic->ID(), *loadedReliquary);
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
            .Type<BasicShard>()
            .Type<MovableOnlyRelic>()
            .Actualize();

        auto savedRelic = savedReliquary->Create<MovableOnlyRelic>();
        savedRelic->myValue = dataGeneration.Random<int>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicShard>()
                .Type<MovableOnlyRelic>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::LocalPtr<MovableOnlyRelic>(savedRelic->ID(), *loadedReliquary);

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
            .Type<BasicCurator>()
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
                .Type<BasicCurator>()
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
        auto savedReliquary = ReliquaryOrigin()
            .Type<BasicSignal>()
            .Actualize();

        savedReliquary->Raise<BasicSignal>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicSignal>()
                .Actualize();

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

SCENARIO_METHOD(ReliquarySerializationTestsFixture, "global alias serialization", "[global][alias][serialization]")
{
    GIVEN("saved reliquary with global relic alias")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Alias<int, GlobalRelic>(
                [](GlobalRelic& backing)
                {
                    return backing.myInt;
                },
                dataGeneration.Random<int>())
            .Actualize();

        auto savedAlias = Arca::AliasPtr<int>(*savedReliquary);

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicShard>()
                .Alias<int, GlobalRelic>(
                    [](GlobalRelic& backing)
                    {
                        return backing.myInt;
                    })
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedAlias = Arca::AliasPtr<int>(*loadedReliquary);

            THEN("loaded relic has saved value")
            {
                REQUIRE(*loadedAlias == *savedAlias);
            }

            THEN("reliquary has global, shard for global, and created signal")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedReliquary->ShardSize() == 1);
                REQUIRE(loadedReliquary->SignalSize() == 1);
            }
        }
    }
}

SCENARIO_METHOD(ReliquarySerializationTestsFixture, "null reliquary serialization", "[reliquary][serialization]")
{
    GIVEN("saved reliquary with global relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Type<BasicShardNullInscription>()
            .Type<GlobalRelicNullInscription>()
            .Actualize();

        auto savedRelic = Arca::GlobalPtr<GlobalRelicNullInscription>(*savedReliquary);
        savedRelic->myInt = dataGeneration.Random<int>();
        savedRelic->basicShard->myValue = dataGeneration.Random<std::string>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicShardNullInscription>()
                .Type<GlobalRelicNullInscription>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::GlobalPtr<GlobalRelicNullInscription>(*loadedReliquary);

            THEN("loaded relic does not have saved value")
            {
                REQUIRE(loadedRelic->myInt != savedRelic->myInt);
            }

            THEN("reliquary only has global")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedReliquary->ShardSize() == 1);
                REQUIRE(loadedReliquary->SignalSize() == 1);
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
            .Type<BasicShardNullInscription>()
            .Type<TypedClosedRelicNullInscription>()
            .Actualize();

        auto savedRelic = savedReliquary->Create<TypedClosedRelicNullInscription>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicShardNullInscription>()
                .Type<TypedClosedRelicNullInscription>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::LocalPtr<TypedClosedRelicNullInscription>(savedRelic->ID(), *loadedReliquary);

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
            .Type<BasicShardNullInscription>()
            .Type<OtherShardNullInscription>()
            .Type<TypedOpenRelicNullInscription>()
            .Actualize();

        auto savedRelic = savedReliquary->Create<TypedOpenRelicNullInscription>();
        auto savedOtherShard = savedRelic->Create<OtherShardNullInscription>();
        savedOtherShard->myValue = dataGeneration.Random<int>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicShardNullInscription>()
                .Type<OtherShardNullInscription>()
                .Type<TypedOpenRelicNullInscription>()
                .Actualize();

            ::Inscription::BinaryArchive::StreamPosition inputArchiveSize = 0;

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
                inputArchiveSize = inputArchive.TellStream();
            }

            auto loadedRelic = Arca::LocalPtr<TypedOpenRelicNullInscription>(savedRelic->ID(), *loadedReliquary);

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
            .Type<BasicCuratorNullInscription>()
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
                .Type<BasicCuratorNullInscription>()
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
        auto savedReliquary = ReliquaryOrigin()
            .Type<BasicSignalNullInscription>()
            .Actualize();

        savedReliquary->Raise<BasicSignalNullInscription>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Type<BasicSignalNullInscription>()
                .Actualize();

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