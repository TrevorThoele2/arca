#include <catch.hpp>
using namespace std::string_literals;

#include "IdentifiedRelicCreationTests.h"

SCENARIO_METHOD(IdentifiedRelicCreationTestsFixture, "identified relic creation", "[relic][identified]")
{
    GIVEN("all types registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Relic>()
            .Register<Shard>()
            .Actualize();

        WHEN("creating identified relic over already occupied ID")
        {
            reliquary->Do(IdentifiedCreate<Relic>{ 2 });

            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(IdentifiedCreate<Relic>{ 2 }),
                    CannotCreate,
                    Catch::Matchers::Message(
                        "The relic (" + TypeFor<Relic>().name + ") cannot be created. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("creating identified relic with valid structure over already occupied ID")
        {
            reliquary->Do(IdentifiedCreate<Relic>{ 2 });

            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(IdentifiedCreateWith<Relic>{2, RelicStructure{ TypeFor<Shard>() }}),
                    CannotCreate,
                    Catch::Matchers::Message(
                        "The relic (" + TypeFor<Relic>().name + ") cannot be created. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("creating identified child relic over already occupied ID")
        {
            auto parentRelic = reliquary->Do(Create<Relic>());
            reliquary->Do(IdentifiedCreate<Relic>{ 2 });

            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(IdentifiedCreateChild<Relic>{ 2, parentRelic }),
                    CannotCreate,
                    Catch::Matchers::Message(
                        "The relic (" + TypeFor<Relic>().name + ") cannot be created. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("creating identified child relic with valid structure over already occupied ID")
        {
            auto parentRelic = reliquary->Do(Create<Relic>());
            reliquary->Do(IdentifiedCreate<Relic>{ 2 });

            THEN("throws")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(IdentifiedCreateChildWith<Relic>{ 2, parentRelic, RelicStructure{ TypeFor<Shard>() }}),
                    CannotCreate,
                    Catch::Matchers::Message(
                        "The relic (" + TypeFor<Relic>().name + ") cannot be created. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("creating identified relic")
        {
            auto preCreateRelicSize = reliquary->RelicSize();
            auto relic = reliquary->Do(IdentifiedCreate<Relic>{ 2 });

            THEN("relic id is requested id")
            {
                REQUIRE(relic.ID() == 2);
            }

            THEN("reliquary has one more relic")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicSize + 1));
            }

            THEN("reliquary contains relic")
            {
                REQUIRE(reliquary->Contains<Relic>(relic.ID()));
            }

            WHEN("finding relic")
            {
                auto found = reliquary->Find<Relic>(relic.ID());

                THEN("found is same as created")
                {
                    REQUIRE(found.ID() == relic.ID());
                    REQUIRE(found.Owner() == relic.Owner());
                }
            }
        }

        WHEN("creating identified relic with valid structure")
        {
            auto preCreateRelicCount = reliquary->RelicSize();
            auto relic = reliquary->Do(IdentifiedCreateWith<Relic>{2, RelicStructure{ TypeFor<Shard>() }});

            THEN("relic id is requested id")
            {
                REQUIRE(relic.ID() == 2);
            }

            THEN("structure has been satisfied")
            {
                REQUIRE(reliquary->Find<Shard>(relic.ID()));
            }

            THEN("reliquary relic count increments by one")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicCount + 1));
            }

            THEN("reliquary contains relic")
            {
                REQUIRE(reliquary->Contains<Relic>(relic.ID()));
            }
        }

        WHEN("creating identified child relic")
        {
            auto parentRelic = reliquary->Do(Create<Relic>());
            auto preCreateRelicSize = reliquary->RelicSize();
            auto relic = reliquary->Do(IdentifiedCreateChild<Relic>{ 2, parentRelic });

            THEN("relic id is requested id")
            {
                REQUIRE(relic.ID() == 2);
            }

            THEN("has parent")
            {
                REQUIRE(reliquary->ParentOf(relic.ID()));
            }

            THEN("reliquary has one more relic")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicSize + 1));
            }

            THEN("reliquary contains relic")
            {
                REQUIRE(reliquary->Contains<Relic>(relic.ID()));
            }
        }

        WHEN("creating identified child relic with valid structure")
        {
            auto parentRelic = reliquary->Do(Create<Relic>());
            auto preCreateRelicSize = reliquary->RelicSize();
            auto relic = reliquary->Do(IdentifiedCreateChildWith<Relic>{ 2, parentRelic, RelicStructure{ TypeFor<Shard>() }} );

            THEN("relic id is requested id")
            {
                REQUIRE(relic.ID() == 2);
            }

            THEN("has parent")
            {
                REQUIRE(reliquary->ParentOf(relic.ID()));
            }

            THEN("structure has been satisfied")
            {
                REQUIRE(reliquary->Find<Shard>(relic.ID()));
            }

            THEN("reliquary has one more relic")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicSize + 1));
            }

            THEN("reliquary contains relic")
            {
                REQUIRE(reliquary->Contains<Relic>(relic.ID()));
            }
        }

        WHEN("creating identified relic over next ID")
        {
            reliquary->Do(IdentifiedCreate<Relic>{ 1 });

            auto nextRelic = reliquary->Do(Create<Relic>());

            THEN("next created relic has advanced ID")
            {
                REQUIRE(nextRelic.ID() == 2);
            }
        }

        WHEN("creating identified relic with valid structure over next ID")
        {
            reliquary->Do(IdentifiedCreateWith<Relic>{1, RelicStructure{ TypeFor<Shard>() }});

            auto nextRelic = reliquary->Do(Create<Relic>());

            THEN("next created relic has advanced ID")
            {
                REQUIRE(nextRelic.ID() == 2);
            }
        }

        WHEN("creating identified child relic over next ID")
        {
            auto parentRelic = reliquary->Do(Create<Relic>());
            reliquary->Do(IdentifiedCreateChild<Relic>{ 2, parentRelic });

            auto nextRelic = reliquary->Do(Create<Relic>());

            THEN("next created relic has advanced ID")
            {
                REQUIRE(nextRelic.ID() == 3);
            }
        }

        WHEN("creating identified child relic with valid structure over next ID")
        {
            auto parentRelic = reliquary->Do(Create<Relic>());
            reliquary->Do(IdentifiedCreateChildWith<Relic>{ 2, parentRelic, RelicStructure{ TypeFor<Shard>() }});

            auto nextRelic = reliquary->Do(Create<Relic>());

            THEN("next created relic has advanced ID")
            {
                REQUIRE(nextRelic.ID() == 3);
            }
        }

        WHEN("creating identified relic over previously used ID")
        {
            auto relic1 = reliquary->Do(Create<Relic>());
            reliquary->Do(Destroy<Relic>{ relic1.ID() });
            auto relic2 = reliquary->Do(Create<Relic>());
            reliquary->Do(Destroy<Relic>{ relic2.ID() });

            auto relic = reliquary->Do(IdentifiedCreate<Relic>{ 1 });

            THEN("created relic has requested ID")
            {
                REQUIRE(relic.ID() == 1);
            }
        }

        WHEN("creating identified relic with valid structure over next ID")
        {
            auto relic1 = reliquary->Do(Create<Relic>());
            reliquary->Do(Destroy<Relic>{ relic1.ID() });
            auto relic2 = reliquary->Do(Create<Relic>());
            reliquary->Do(Destroy<Relic>{ relic2.ID() });

            auto relic = reliquary->Do(IdentifiedCreateWith<Relic>{1, RelicStructure{ TypeFor<Shard>() }});

            THEN("created relic has requested ID")
            {
                REQUIRE(relic.ID() == 1);
            }
        }

        WHEN("creating identified child relic over next ID")
        {
            auto relic1 = reliquary->Do(Create<Relic>());
            reliquary->Do(Destroy<Relic>{ relic1.ID() });
            auto relic2 = reliquary->Do(Create<Relic>());
            reliquary->Do(Destroy<Relic>{ relic2.ID() });

            auto parentRelic = reliquary->Do(Create<Relic>());
            auto relic = reliquary->Do(IdentifiedCreateChild<Relic>{ 1, parentRelic });

            THEN("created relic has requested ID")
            {
                REQUIRE(relic.ID() == 1);
            }
        }

        WHEN("creating identified child relic with valid structure over next ID")
        {
            auto relic1 = reliquary->Do(Create<Relic>());
            reliquary->Do(Destroy<Relic>{ relic1.ID() });
            auto relic2 = reliquary->Do(Create<Relic>());
            reliquary->Do(Destroy<Relic>{ relic2.ID() });

            auto parentRelic = reliquary->Do(Create<Relic>());
            auto relic = reliquary->Do(IdentifiedCreateChildWith<Relic>{ 1, parentRelic, RelicStructure{ TypeFor<Shard>() }});

            THEN("created relic has requested ID")
            {
                REQUIRE(relic.ID() == 1);
            }
        }
    }
}