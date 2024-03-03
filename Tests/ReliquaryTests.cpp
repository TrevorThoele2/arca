#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "ReliquaryTests.h"

ReliquaryTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

ReliquaryTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

void ReliquaryTestsFixture::BasicTypedRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void ReliquaryTestsFixture::GlobalRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

namespace Arca
{
    const TypeName Traits<::ReliquaryTestsFixture::BasicShard>::typeName =
        "ReliquaryTestsBasicShard";

    const TypeName Traits<::ReliquaryTestsFixture::OtherBasicShard>::typeName =
        "ReliquaryTestsOtherBasicShard";

    const TypeName Traits<::ReliquaryTestsFixture::OtherShard>::typeName =
        "ReliquaryTestsOtherShard";

    const TypeName Traits<::ReliquaryTestsFixture::BasicTypedRelic>::typeName =
        "ReliquaryTestsBasicTypedRelic";

    const TypeName Traits<::ReliquaryTestsFixture::GlobalRelic>::typeName =
        "ReliquaryTestsGlobalRelic";

    const TypeName Traits<::ReliquaryTestsFixture::BasicCurator>::typeName =
        "ReliquaryTestsBasicCurator";

    const TypeName Traits<::ReliquaryTestsFixture::BasicSignal>::typeName =
        "ReliquaryTestsBasicSignal";
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
            auto found = reliquary->Find<OpenRelic>(1);

            THEN("is empty")
            {
                REQUIRE(!found);
            }
        }

        WHEN("querying contains relic")
        {
            auto contains = reliquary->Contains<OpenRelic>(1);

            THEN("is false")
            {
                REQUIRE(!contains);
            }
        }

        WHEN("creating relic")
        {
            reliquary->Create<OpenRelic>();

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

SCENARIO_METHOD(ReliquaryTestsFixture, "reliquary wih single shard")
{
    GIVEN("reliquary registered with single shard")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Actualize();

        WHEN("creating open relic with non-const shard")
        {
            auto relic = reliquary->Create<OpenRelic>();
            auto shard = relic->Create<BasicShard>();

            THEN("creating const shard of same type throws")
            {
                REQUIRE_THROWS_AS(relic->Create<const BasicShard>(), CannotCreate);
            }

            THEN("finding either shard gives correct")
            {
                auto found = reliquary->Find<Either<BasicShard>>(relic->ID());
                REQUIRE(found);
                REQUIRE(&*found == &*shard);
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "creating relic from registered relic structure", "[reliquary]")
{
    GIVEN("reliquary registered with structure")
    {
        const auto structureName = dataGeneration.Random<std::string>();
        const auto relicStructure = RelicStructure{ TypeFor<BasicShard>() };

        auto reliquaryOrigin = ReliquaryOrigin()
            .RelicStructure(structureName, relicStructure);

        WHEN("constructing relic from registered structure")
        {
            auto reliquary = reliquaryOrigin
                .Type<BasicShard>()
                .Actualize();

            auto relic = reliquary->CreateWith<ClosedRelic>(structureName);

            THEN("relic has shard")
            {
                REQUIRE(relic->Find<BasicShard>());
                REQUIRE(relic->Contains<BasicShard>());
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
                    reliquary->CreateWith<ClosedRelic>(structureName),
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
            .Type<BasicTypedRelic>()
            .Type<GlobalRelic>()
            .RelicStructure(dataGeneration.Random<std::string>(), RelicStructure())
            .Type<BasicShard>()
            .Type<BasicCurator>()
            .CuratorPipeline(Pipeline())
            .Type<BasicSignal>()
            .Actualize();

        WHEN("checking relic size")
        {
            THEN("has one relic for global relic")
            {
                REQUIRE(reliquary->RelicSize() == 1);
            }

            THEN("has one shard for global relic")
            {
                REQUIRE(reliquary->ShardSize() == 1);
            }
        }
    }
}