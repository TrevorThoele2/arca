#include <catch.hpp>

#include "GlobalRelicTests.h"

#include <Arca/RelicParented.h>

GlobalRelicTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

void GlobalRelicTestsFixture::BasicTypedRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void GlobalRelicTestsFixture::GlobalRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
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

        auto& test = *static_cast<Reliquary*>(nullptr);

        WHEN("retrieving global relic")
        {
            const auto globalRelic = Arca::GlobalPtr<GlobalRelic>(*reliquary);

            THEN("structure has been satisfied")
            {
                REQUIRE(Arca::LocalPtr<BasicShard>(globalRelic->ID(), globalRelic->Owner()));
                REQUIRE(globalRelic->basicShard);
            }

            THEN("cannot destroy relic")
            {
                auto preDestroyRelicCount = reliquary->RelicSize();

                reliquary->Destroy(AsHandle(*globalRelic));

                auto foundAgain = Arca::GlobalPtr<GlobalRelic>(*reliquary);

                REQUIRE(foundAgain->ID() == globalRelic->ID());
                REQUIRE(foundAgain->basicShard == globalRelic->basicShard);
                REQUIRE(preDestroyRelicCount == reliquary->RelicSize());
            }

            WHEN("retrieving global relic as open")
            {
                const auto asOpen = Arca::LocalPtr<OpenRelic>(globalRelic->ID(), globalRelic->Owner());

                THEN("open is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as closed")
            {
                const auto asClosed = Arca::LocalPtr<ClosedRelic>(globalRelic->ID(), globalRelic->Owner());

                THEN("closed is empty")
                {
                    REQUIRE(!asClosed);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::LocalPtr<BasicTypedRelic>(globalRelic->ID(), globalRelic->Owner());

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
            .Type<BasicShard>()
            .Type<GlobalRelic>();

        WHEN("actualizing and creating typed relic with type from global relic")
        {
            auto reliquary = origin.Actualize();

            THEN("throws error")
            {
                REQUIRE_THROWS(reliquary->Create<GlobalRelic>());
            }
        }

        WHEN("registering int computation with global relic backing")
        {
            THEN("not throws error")
            {
                REQUIRE_NOTHROW(origin.Compute<int>(
                    [](Reliquary& reliquary)
                    {
                        const GlobalPtr<GlobalRelic> backing(reliquary);
                        return backing->myValue;
                    }));
            }
        }
    }

    GIVEN("global computation registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Type<BasicTypedRelic>()
            .Type<GlobalRelic>()
            .Compute<int>(
                [](Reliquary& reliquary)
                {
                    const GlobalPtr<GlobalRelic> backing(reliquary);
                    return backing->myValue;
                })
            .Actualize();

        WHEN("retrieving backing relic type")
        {
            GlobalPtr<GlobalRelic> global(*reliquary);

            THEN("is retrieved")
            {
                REQUIRE(static_cast<bool>(global));
            }
        }
    }
}