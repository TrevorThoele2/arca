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
    using Shards = ShardsFor<BasicTypedRelic>;
    auto tuple = ExtractShards<Shards>(ID(), Owner());
    basicShard = std::get<0>(tuple);
}

ReliquaryTestsFixture::StaticRelic::StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data) :
    TypedRelic(data.base)
{}

void ReliquaryTestsFixture::StaticRelic::InitializeImplementation()
{
    using Shards = ShardsFor<StaticRelic>;
    auto tuple = ExtractShards<Shards>(ID(), Owner());
    basicShard = std::get<0>(tuple);
}

namespace Arca
{
    const TypeHandle ShardTraits<::ReliquaryTestsFixture::BasicShard>::typeHandle =
        "ReliquaryTestsBasicShard";

    const TypeHandle ShardTraits<::ReliquaryTestsFixture::OtherBasicShard>::typeHandle =
        "ReliquaryTestsOtherBasicShard";

    const TypeHandle ShardTraits<::ReliquaryTestsFixture::OtherShard>::typeHandle =
        "ReliquaryTestsOtherShard";

    const TypeHandle RelicTraits<::ReliquaryTestsFixture::BasicTypedRelic>::typeHandle =
        "ReliquaryTestsBasicTypedRelic";

    const TypeHandle RelicTraits<::ReliquaryTestsFixture::StaticRelic>::typeHandle =
        "ReliquaryTestsStaticRelic";

    const TypeHandle CuratorTraits<::ReliquaryTestsFixture::BasicCurator>::typeHandle =
        "ReliquaryTestsBasicCurator";
}

SCENARIO_METHOD(ReliquaryTestsFixture, "default reliquary", "[reliquary]")
{
    GIVEN("default reliquary")
    {
        auto reliquary = ReliquaryOrigin().Actualize();

        WHEN("checking relic count")
        {
            THEN("has relic count of zero")
            {
                REQUIRE(reliquary.RelicSize() == 0);
            }
        }

        WHEN("finding relic")
        {
            auto found = reliquary.FindRelic(1);

            THEN("is empty")
            {
                REQUIRE(!found.has_value());
            }
        }

        WHEN("finding shard")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.Find<BasicShard>(1),
                    NotRegistered,
                    Catch::Matchers::Message("The shard (ReliquaryTestsBasicShard) was not registered.")
                );
            }
        }

        WHEN("creating relic")
        {
            reliquary.CreateRelic();

            THEN("reliquary has relic count of one")
            {
                REQUIRE(reliquary.RelicSize() == 1);
            }
        }

        WHEN("creating typed relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.Create<BasicTypedRelic>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The relic ("s + RelicTraits<BasicTypedRelic>::typeHandle + ") was not registered.")
                );
            }
        }

        WHEN("retrieving static relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.Static<StaticRelic>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The static relic ("s + RelicTraits<StaticRelic>::typeHandle + ") was not registered.")
                );
            }
        }

        WHEN("finding curator")
        {
            THEN("returns nullptr")
            {
                REQUIRE(reliquary.Find<BasicCurator>() == nullptr);
            }
        }

        WHEN("finding curator const")
        {
            const auto& constReliquary = reliquary;

            THEN("returns nullptr")
            {
                REQUIRE(constReliquary.Find<BasicCurator>() == nullptr);
            }
        }

        WHEN("raising signal")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.RaiseSignal(BasicSignal()),
                    NotRegistered,
                    Catch::Matchers::Message("The signal ("s + typeid(BasicSignal).name() + ") was not registered.")
                );
            }
        }
    }

    GIVEN("reliquary with typed relic registered")
    {
        auto reliquary = ReliquaryOrigin().Relic<BasicTypedRelic>().Actualize();

        WHEN("creating typed relic without shard being registered")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.Create<BasicTypedRelic>(),
                    NotRegistered,
                    Catch::Matchers::Message(
                        "The shard ("s + ShardTraits<BasicShard>::typeHandle + ") was not registered.")
                );
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "registering relic structure with same name twice", "[ReliquaryOrigin]")
{
    GIVEN("reliquary origin")
    {
        auto reliquaryOrigin = ReliquaryOrigin();

        WHEN("registering rleic structure")
        {
            const auto name = dataGeneration.Random<std::string>();
            reliquaryOrigin.RelicStructure(name, RelicStructure());

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquaryOrigin.RelicStructure(name, RelicStructure()), AlreadyRegistered);
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "creating relic from registered relic structure", "[reliquary]")
{
    GIVEN("reliquary registered with structure")
    {
        const auto structureName = dataGeneration.Random<std::string>();
        const auto relicStructure = RelicStructure{ ShardTraits<BasicShard>::typeHandle };

        auto reliquaryOrigin = ReliquaryOrigin()
            .RelicStructure(structureName, relicStructure);

        WHEN("constructing relic from registered structure")
        {
            auto reliquary = reliquaryOrigin
                .Shard<BasicShard>()
                .Actualize();

            auto relic = reliquary.CreateRelic(structureName);

            THEN("relic has shard")
            {
                REQUIRE(relic.FindShard<BasicShard>() != nullptr);
                REQUIRE(relic.HasShard<BasicShard>());
            }

            THEN("relic is fixed")
            {
                REQUIRE(relic.Dynamism() == RelicDynamism::Fixed);
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
                    reliquary.CreateRelic(structureName),
                    NotRegistered,
                    ::Catch::Matchers::Message(
                        "The shard ("s + ShardTraits<BasicShard>::typeHandle + ") was not registered.")
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
            .CuratorPipeline(CuratorPipeline())
            .Signal<BasicSignal>()
            .Actualize();

        WHEN("checking relic count")
        {
            THEN("has count of one for static shard")
            {
                REQUIRE(reliquary.RelicSize() == 1);
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
            .CuratorPipeline(CuratorPipeline())
            .Signal<BasicSignal>()
            .Actualize();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Shard<BasicShard>()
                .StaticRelic<StaticRelic>()
                .Curator<BasicCurator>()
                .CuratorPipeline(CuratorPipeline())
                .Signal<BasicSignal>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(loadedReliquary);
            }

            THEN("has only static relic")
            {
                REQUIRE(loadedReliquary.RelicSize() == 1);
            }
        }
    }

    GIVEN("saved reliquary with dynamic relic")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Actualize();

        auto savedRelic = savedReliquary.CreateRelic();
        auto savedShard = savedRelic.CreateShard<BasicShard>();
        savedShard->myValue = dataGeneration.Random<std::string>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Shard<BasicShard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(loadedReliquary);
            }

            auto loadedRelic = loadedReliquary.FindRelic(savedRelic.ID());
            auto shardFromRelic = loadedRelic->FindShard<BasicShard>();

            THEN("has relic")
            {
                REQUIRE(loadedReliquary.RelicSize() == 1);
                REQUIRE(loadedRelic.has_value());
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = loadedReliquary.Find<BasicShard>(loadedRelic->ID());
                REQUIRE(shardFromReliquary != nullptr);
                REQUIRE(shardFromRelic != nullptr);
                REQUIRE(loadedRelic->HasShard<BasicShard>());
            }

            THEN("shard has saved value")
            {
                REQUIRE(shardFromRelic->myValue == savedShard->myValue);
            }

            THEN("relic is dynamic")
            {
                REQUIRE(loadedRelic->Dynamism() == RelicDynamism::Dynamic);
            }

            THEN("relic owner is loaded reliquary")
            {
                REQUIRE(&loadedRelic->Owner() == &loadedReliquary);
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
                inputArchive(loadedReliquary);
            }

            auto loadedRelic = loadedReliquary.FindRelic(savedRelic.ID());

            THEN("has relic")
            {
                REQUIRE(loadedReliquary.RelicSize() == 1);
                REQUIRE(loadedRelic.has_value());
            }

            THEN("relic does not have shard")
            {
                REQUIRE_THROWS_AS(loadedRelic->FindShard<BasicShard>(), NotRegistered);
                REQUIRE_THROWS_AS(loadedReliquary.Find<BasicShard>(loadedRelic->ID()), NotRegistered);
            }

            THEN("relic is dynamic")
            {
                REQUIRE(loadedRelic->Dynamism() == RelicDynamism::Dynamic);
            }

            THEN("relic owner is loaded reliquary")
            {
                REQUIRE(&loadedRelic->Owner() == &loadedReliquary);
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
                inputArchive(loadedReliquary);
            }

            auto loadedRelic = loadedReliquary.FindRelic(savedRelic.ID());
            auto shardFromRelic = loadedRelic->FindShard<OtherBasicShard>();

            THEN("has relic")
            {
                REQUIRE(loadedReliquary.RelicSize() == 1);
                REQUIRE(loadedRelic.has_value());
            }

            THEN("relic has shard")
            {
                auto shardFromReliquary = loadedReliquary.Find<OtherBasicShard>(loadedRelic->ID());
                REQUIRE(shardFromReliquary != nullptr);
                REQUIRE(shardFromRelic != nullptr);
                REQUIRE(loadedRelic->HasShard<OtherBasicShard>());
            }

            THEN("shard has saved value")
            {
                REQUIRE(shardFromRelic->myValue == savedShard->myValue);
            }

            THEN("relic is dynamic")
            {
                REQUIRE(loadedRelic->Dynamism() == RelicDynamism::Dynamic);
            }

            THEN("relic owner is loaded reliquary")
            {
                REQUIRE(&loadedRelic->Owner() == &loadedReliquary);
            }

            THEN("relic id is saved id")
            {
                REQUIRE(loadedRelic->ID() == savedRelic.ID());
            }
        }
    }
}