#include <catch.hpp>

#include "GlobalRelicTests.h"

#include <Arca/RelicParented.h>

GlobalRelicTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

void GlobalRelicTestsFixture::BasicTypedRelic::InitializeImplementation()
{
    auto shards = ExtractShards();
    basicShard = std::get<0>(shards);
}

void GlobalRelicTestsFixture::GlobalRelic::InitializeImplementation()
{
    auto shards = ExtractShards();
    basicShard = std::get<0>(shards);
}

namespace Arca
{
    const TypeHandleName Traits<GlobalRelicTestsFixture::BasicShard>::typeName =
        "RelicTestsBasicShard";

    const TypeHandleName Traits<GlobalRelicTestsFixture::BasicTypedRelic>::typeName =
        "ReliquaryTestsBasicTypedRelic";

    const TypeHandleName Traits<GlobalRelicTestsFixture::GlobalRelic>::typeName =
        "ReliquaryTestsGlobalRelic";
}

SCENARIO_METHOD(GlobalRelicTestsFixture, "global relic", "[relic][global]")
{
    GIVEN("all types registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Type<BasicTypedRelic>()
            .Type<GlobalRelic>()
            .Actualize();

        WHEN("retrieving global relic")
        {
            const auto globalRelic = reliquary->Find<GlobalRelic>();

            THEN("structure has been satisfied")
            {
                REQUIRE(reliquary->Find<BasicShard>(globalRelic->ID()));
                REQUIRE(globalRelic->basicShard);
            }

            THEN("cannot destroy relic")
            {
                auto preDestroyRelicCount = reliquary->RelicSize();

                reliquary->Destroy(globalRelic);

                auto foundAgain = reliquary->Find<GlobalRelic>();

                REQUIRE(foundAgain->ID() == globalRelic->ID());
                REQUIRE(foundAgain->basicShard == globalRelic->basicShard);
                REQUIRE(preDestroyRelicCount == reliquary->RelicSize());
            }

            WHEN("retrieving global relic as open")
            {
                const auto asOpen = reliquary->Find<OpenRelic>(globalRelic->ID());

                THEN("open is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as closed")
            {
                const auto asClosed = reliquary->Find<ClosedRelic>(globalRelic->ID());

                THEN("closed is empty")
                {
                    REQUIRE(!asClosed);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = reliquary->Find<BasicTypedRelic>(globalRelic->ID());

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }
    }

    GIVEN("global relic registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Type<GlobalRelic>()
            .Actualize();

        WHEN("creating typed relic with type from global relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS(reliquary->Create<GlobalRelic>());
            }
        }
    }
}