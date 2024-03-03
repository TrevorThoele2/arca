#include <catch.hpp>

#include "GlobalRelicTests.h"

#include <Arca/OpenRelic.h>

GlobalRelicTestsFixture::BasicTypedRelic::BasicTypedRelic(RelicInit init)
{
    basicShard = init.Create<BasicShard>();
}

GlobalRelicTestsFixture::GlobalRelic::GlobalRelic(RelicInit init)
{
    basicShard = init.Create<BasicShard>();
}

SCENARIO_METHOD(GlobalRelicTestsFixture, "global relic", "[relic][global]")
{
    GIVEN("all types registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Register<BasicTypedRelic>()
            .Register<GlobalRelic>()
            .Actualize();

        WHEN("retrieving global relic")
        {
            const auto globalRelic = reliquary->Find<GlobalRelic>();

            THEN("structure has been satisfied")
            {
                REQUIRE(reliquary->Find<BasicShard>(globalRelic.ID()));
                REQUIRE(globalRelic->basicShard);
            }

            THEN("throws when trying to destroy relic through handle")
            {
                REQUIRE_THROWS_AS(reliquary->Destroy(AsHandle(globalRelic)), NotRegistered);
            }

            WHEN("retrieving global relic as open")
            {
                const auto asOpen = reliquary->Find<OpenRelic>(globalRelic.ID());

                THEN("open is empty")
                {
                    REQUIRE(!asOpen);
                }
            }
            
            WHEN("retrieving as typed")
            {
                const auto asTyped = reliquary->Find<BasicTypedRelic>(globalRelic.ID());

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }
    }
}