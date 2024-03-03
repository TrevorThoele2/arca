#include <catch.hpp>

#include "GlobalRelicTests.h"

GlobalRelicTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

GlobalRelicTestsFixture::BasicTypedRelic::BasicTypedRelic(Init init)
    : ClosedTypedRelic(init)
{
    basicShard = FindOrCreate<BasicShard>();
}

GlobalRelicTestsFixture::GlobalRelic::GlobalRelic(Init init)
    : ClosedTypedRelic(init)
{
    basicShard = FindOrCreate<BasicShard>();
}

SCENARIO_METHOD(GlobalRelicTestsFixture, "global relic", "[relic][global]")
{
    GIVEN("all types registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<BasicTypedRelic>()
            .Register<GlobalRelic>()
            .Actualize();

        WHEN("retrieving global relic")
        {
            const auto globalRelic = Arca::GlobalIndex<GlobalRelic>(*reliquary);

            THEN("structure has been satisfied")
            {
                REQUIRE(Arca::ShardIndex<BasicShard>(globalRelic->ID(), globalRelic->Owner()));
                REQUIRE(globalRelic->basicShard);
            }

            THEN("throws when trying to destroy relic through handle")
            {
                REQUIRE_THROWS_AS(reliquary->Destroy(AsHandle(*globalRelic)), NotRegistered);
            }

            WHEN("retrieving global relic as open")
            {
                const auto asOpen = Arca::RelicIndex<OpenRelic>(globalRelic->ID(), globalRelic->Owner());

                THEN("open is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as closed")
            {
                const auto asClosed = Arca::RelicIndex<ClosedRelic>(globalRelic->ID(), globalRelic->Owner());

                THEN("closed is empty")
                {
                    REQUIRE(!asClosed);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::RelicIndex<BasicTypedRelic>(globalRelic->ID(), globalRelic->Owner());

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }
    }

    GIVEN("global relic registered")
    {
        auto origin = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<GlobalRelic>();

        WHEN("registering int postulate with global relic backing")
        {
            THEN("not throws error")
            {
                REQUIRE_NOTHROW(origin.Postulate<int>(
                    [](Reliquary& reliquary)
                    {
                        const GlobalIndex<GlobalRelic> backing(reliquary);
                        return backing->myValue;
                    }));
            }
        }
    }

    GIVEN("postulate registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<BasicTypedRelic>()
            .Register<GlobalRelic>()
            .Postulate<int>(
                [](Reliquary& reliquary)
                {
                    const GlobalIndex<GlobalRelic> backing(reliquary);
                    return backing->myValue;
                })
            .Actualize();

        WHEN("retrieving backing relic type")
        {
            GlobalIndex<GlobalRelic> global(*reliquary);

            THEN("is retrieved")
            {
                REQUIRE(static_cast<bool>(global));
            }
        }
    }
}