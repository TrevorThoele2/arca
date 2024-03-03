#include <catch.hpp>

#include "GlobalRelicTests.h"

#include <Arca/LocalRelic.h>

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
            .Register<ClosedRelic>()
            .Register<BasicShard>()
            .Register<BasicTypedRelic>()
            .Register<GlobalRelic>()
            .Actualize();

        WHEN("retrieving global relic")
        {
            const auto globalRelic = Arca::Index<GlobalRelic>(*reliquary);

            THEN("structure has been satisfied")
            {
                REQUIRE(Arca::Index<BasicShard>(globalRelic.ID(), *reliquary));
                REQUIRE(globalRelic->basicShard);
            }

            THEN("throws when trying to destroy relic through handle")
            {
                REQUIRE_THROWS_AS(reliquary->Destroy(AsHandle(globalRelic)), NotRegistered);
            }

            WHEN("retrieving global relic as open")
            {
                const auto asOpen = Arca::Index<OpenRelic>(globalRelic.ID(), *reliquary);

                THEN("open is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as closed")
            {
                const auto asClosed = Arca::Index<ClosedRelic>(globalRelic.ID(), *reliquary);

                THEN("closed is empty")
                {
                    REQUIRE(!asClosed);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::Index<BasicTypedRelic>(globalRelic.ID(), *reliquary);

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }
    }
}