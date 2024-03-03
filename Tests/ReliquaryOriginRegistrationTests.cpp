#include <catch.hpp>
using namespace std::string_literals;

#include "ReliquaryOriginRegistrationTests.h"

ReliquaryOriginRegistrationTestsFixture::GlobalRelicWithMovedValue::GlobalRelicWithMovedValue(
    std::unique_ptr<int>&& myInt)
    :
    myInt(std::move(myInt))
{}

ReliquaryOriginRegistrationTestsFixture::CuratorWithMovedValue::CuratorWithMovedValue(
    Init init, std::unique_ptr<int>&& myInt)
    :
    Curator(init), myInt(std::move(myInt))
{}

SCENARIO_METHOD(ReliquaryOriginRegistrationTestsFixture, "registering types multiple times", "[reliquaryorigin]")
{
    GIVEN("all types registered to origin")
    {
        const auto relicStructureName = dataGeneration.Random<std::string>();

        auto reliquaryOrigin = ReliquaryOrigin()
            .Register<Shard>()
            .Register<Relic>()
            .Register<GlobalRelic>()
            .Register<Curator>()
            .RelicStructure(relicStructureName, RelicStructure{});

        WHEN("registering shard again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Register<Shard>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The shard (" + TypeFor<Shard>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(Shard).name() + "\".")
                );
            }
        }

        WHEN("registering relic again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Register<Relic>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The relic (" + TypeFor<Relic>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(Relic).name() + "\".")
                );
            }
        }

        WHEN("registering global relic again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Register<GlobalRelic>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The global relic (" + TypeFor<GlobalRelic>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(GlobalRelic).name() + "\".")
                );
            }
        }

        WHEN("registering curator again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.Register<Curator>(),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The curator (" + TypeFor<Curator>().name + ") was already registered. " +
                        "The class name is: \"" + typeid(Curator).name() + "\".")
                );
            }
        }

        WHEN("registering relic structure again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquaryOrigin.RelicStructure(relicStructureName, RelicStructure{}),
                    AlreadyRegistered,
                    Catch::Matchers::Message(
                        "The relic structure (" + relicStructureName + ") was already registered.")
                );
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryOriginRegistrationTestsFixture, "registering types with moved values", "[reliquaryorigin]")
{
    GIVEN("registered reliquary origin with types requiring moved values")
    {
        auto globalIntValue = dataGeneration.Random<int>();
        auto globalValue = std::make_unique<int>(globalIntValue);

        auto curatorIntValue = dataGeneration.Random<int>();
        auto curatorValue = std::make_unique<int>(curatorIntValue);

        auto reliquaryOrigin = ReliquaryOrigin()
            .Register<GlobalRelicWithMovedValue>(std::move(globalValue))
            .Register<CuratorWithMovedValue>(std::move(curatorValue));

        WHEN("actualizing")
        {
            auto reliquary = reliquaryOrigin.Actualize();

            THEN("has values")
            {
                auto global = Arca::Index<GlobalRelicWithMovedValue>(*reliquary);
                REQUIRE(*global->myInt == globalIntValue);

                auto& curator = reliquary->Find<CuratorWithMovedValue>();
                REQUIRE(*curator.myInt == curatorIntValue);
            }
        }
    }
}