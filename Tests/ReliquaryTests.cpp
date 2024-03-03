#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "ReliquaryTests.h"

#include <Arca/ExtractShards.h>

ReliquaryTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

ReliquaryTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

ReliquaryTestsFixture::BasicTypedRelic::BasicTypedRelic(const ::Inscription::BinaryTableData<BasicTypedRelic>& data) :
    TypedRelic(data.base)
{}

void ReliquaryTestsFixture::BasicTypedRelic::InitializeImplementation()
{
    using Shards = shards_for_t<BasicTypedRelic>;
    auto tuple = ExtractShards<Shards>(ID(), Owner());
    basicShard = std::get<0>(tuple);
}

ReliquaryTestsFixture::StaticRelic::StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data) :
    TypedRelic(data.base)
{}

void ReliquaryTestsFixture::StaticRelic::InitializeImplementation()
{
    using Shards = shards_for_t<StaticRelic>;
    auto tuple = ExtractShards<Shards>(ID(), Owner());
    basicShard = std::get<0>(tuple);
}

namespace Arca
{
    const TypeHandleName Traits<::ReliquaryTestsFixture::BasicShard>::typeName =
        "ReliquaryTestsBasicShard";

    const TypeHandleName Traits<::ReliquaryTestsFixture::OtherBasicShard>::typeName =
        "ReliquaryTestsOtherBasicShard";

    const TypeHandleName Traits<::ReliquaryTestsFixture::OtherShard>::typeName =
        "ReliquaryTestsOtherShard";

    const TypeHandleName Traits<::ReliquaryTestsFixture::BasicTypedRelic>::typeName =
        "ReliquaryTestsBasicTypedRelic";

    const TypeHandleName Traits<::ReliquaryTestsFixture::StaticRelic>::typeName =
        "ReliquaryTestsStaticRelic";

    const TypeHandleName Traits<::ReliquaryTestsFixture::BasicCurator>::typeName =
        "ReliquaryTestsBasicCurator";
}

SCENARIO_METHOD(ReliquaryTestsFixture, "default reliquary", "[reliquary]")
{
    GIVEN("default reliquary")
    {
        auto reliquary = ReliquaryOrigin().Actualize();

        WHEN("checking relic size")
        {
            THEN("has size of zero")
            {
                REQUIRE(reliquary->RelicSize() == 0);
            }
        }

        WHEN("checking shard size")
        {
            THEN("has size of zero")
            {
                REQUIRE(reliquary->ShardSize() == 0);
            }
        }

        WHEN("checking curator size")
        {
            THEN("has size of zero")
            {
                REQUIRE(reliquary->CuratorSize() == 0);
            }
        }

        WHEN("checking signal size")
        {
            THEN("has size of zero")
            {
                REQUIRE(reliquary->SignalSize() == 0);
            }
        }

        WHEN("finding relic")
        {
            auto found = reliquary->Find<DynamicRelic>(1);

            THEN("is empty")
            {
                REQUIRE(!found);
            }
        }

        WHEN("creating relic")
        {
            reliquary->Create<DynamicRelic>();

            THEN("reliquary has relic count of one")
            {
                REQUIRE(reliquary->RelicSize() == 1);
            }
        }

        WHEN("finding curator")
        {
            THEN("returns nullptr")
            {
                REQUIRE(reliquary->Find<BasicCurator>() == nullptr);
            }
        }

        WHEN("finding curator const")
        {
            const auto& constReliquary = reliquary;

            THEN("returns nullptr")
            {
                REQUIRE(constReliquary->Find<BasicCurator>() == nullptr);
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "creating relic from registered relic structure", "[reliquary]")
{
    GIVEN("reliquary registered with structure")
    {
        const auto structureName = dataGeneration.Random<std::string>();
        const auto relicStructure = RelicStructure{ TypeHandleFor<BasicShard>() };

        auto reliquaryOrigin = ReliquaryOrigin()
            .RelicStructure(structureName, relicStructure);

        WHEN("constructing relic from registered structure")
        {
            auto reliquary = reliquaryOrigin
                .Shard<BasicShard>()
                .Actualize();

            auto relic = reliquary->Create<FixedRelic>(structureName);

            THEN("relic has shard")
            {
                REQUIRE(relic.Find<BasicShard>());
                REQUIRE(relic.Has<BasicShard>());
            }
        }

        WHEN("constructing relic without registering shard")
        {
            auto reliquary = reliquaryOrigin
                .Actualize();

            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Create<FixedRelic>(structureName),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The shard ("s + Traits<BasicShard>::typeName + ") was not registered.")
                );
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "registered reliquary with every type", "[reliquary]")
{
    GIVEN("all registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<BasicTypedRelic>()
            .StaticRelic<StaticRelic>()
            .RelicStructure(dataGeneration.Random<std::string>(), RelicStructure())
            .Shard<BasicShard>()
            .Curator<BasicCurator>()
            .CuratorPipeline(Pipeline())
            .Signal<BasicSignal>()
            .Actualize();

        WHEN("checking relic size")
        {
            THEN("has one relic for static relic")
            {
                REQUIRE(reliquary->RelicSize() == 1);
            }

            THEN("has one shard for static relic")
            {
                REQUIRE(reliquary->ShardSize() == 1);
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "reliquary serialization", "[reliquary][serialization]")
{
    GIVEN("saved empty reliquary with every type registered")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .StaticRelic<StaticRelic>()
            .Curator<BasicCurator>()
            .CuratorPipeline(Pipeline())
            .Signal<BasicSignal>()
            .Actualize();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Shard<BasicShard>()
                .StaticRelic<StaticRelic>()
                .Curator<BasicCurator>()
                .CuratorPipeline(Pipeline())
                .Signal<BasicSignal>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(*loadedReliquary);
            }

            THEN("has only static relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
            }
        }
    }

    GIVEN("saved reliquary with dynamic relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Actualize();

        auto savedRelic = savedReliquary->Create<DynamicRelic>();
        auto savedShard = savedRelic.Create<BasicShard>();
        savedShard->myValue = dataGeneration.Random<std::string>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Shard<BasicShard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = loadedReliquary->Find<DynamicRelic>(savedRelic.ID());
            auto shardFromRelic = loadedRelic->Find<BasicShard>();

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = loadedReliquary->Find<BasicShard>(loadedRelic->ID());
                REQUIRE(shardFromReliquary);
                REQUIRE(shardFromRelic);
                REQUIRE(loadedRelic->Has<BasicShard>());
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
                REQUIRE(loadedRelic->ID() == savedRelic.ID());
            }
        }

        WHEN("loading reliquary without registering shard type")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = loadedReliquary->Find<DynamicRelic>(savedRelic.ID());

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
            }

            THEN("relic does not have shard")
            {
                REQUIRE_THROWS_AS(loadedRelic->Find<BasicShard>(), NotRegistered);
                REQUIRE_THROWS_AS(loadedReliquary->Find<BasicShard>(loadedRelic->ID()), NotRegistered);
            }

            THEN("relic owner is loaded reliquary")
            {
                REQUIRE(&loadedRelic->Owner() == loadedReliquary.get());
            }

            THEN("relic id is saved id")
            {
                REQUIRE(loadedRelic->ID() == savedRelic.ID());
            }
        }

        WHEN("loading reliquary with different shard type with same input type handle")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Shard<OtherBasicShard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = loadedReliquary->Find<DynamicRelic>(savedRelic.ID());
            auto shardFromRelic = loadedRelic->Find<OtherBasicShard>();

            THEN("has relic")
            {
                REQUIRE(loadedReliquary->RelicSize() == 1);
                REQUIRE(loadedRelic);
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = loadedReliquary->Find<OtherBasicShard>(loadedRelic->ID());
                REQUIRE(shardFromReliquary);
                REQUIRE(shardFromRelic);
                REQUIRE(loadedRelic->Has<OtherBasicShard>());
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
                REQUIRE(loadedRelic->ID() == savedRelic.ID());
            }
        }
    }
}