#include <catch.hpp>

#include "IsTypeTests.h"

#include <Arca/IsType.h>

SCENARIO_METHOD(IsTypeTestsFixture, "is type", "[IsType]")
{
    GIVEN("shard type name")
    {
        auto typeName = TypeFor<Shard>().name;

        WHEN("checking is shard")
        {
            THEN("returns true")
            {
                REQUIRE(IsType<Shard>(typeName));
            }
        }

        WHEN("checking is relic")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Relic>(typeName));
            }
        }

        WHEN("checking is curator")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Curator>(typeName));
            }
        }
    }

    GIVEN("relic type name")
    {
        auto typeName = TypeFor<Relic>().name;

        WHEN("checking is shard")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Shard>(typeName));
            }
        }

        WHEN("checking is relic")
        {
            THEN("returns true")
            {
                REQUIRE(IsType<Relic>(typeName));
            }
        }

        WHEN("checking is curator")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Curator>(typeName));
            }
        }
    }

    GIVEN("curator type name")
    {
        auto typeName = TypeFor<Curator>().name;

        WHEN("checking is shard")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Shard>(typeName));
            }
        }

        WHEN("checking is relic")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Relic>(typeName));
            }
        }

        WHEN("checking is curator")
        {
            THEN("returns true")
            {
                REQUIRE(IsType<Curator>(typeName));
            }
        }
    }
}