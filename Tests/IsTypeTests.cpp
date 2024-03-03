#include <catch.hpp>

#include "IsTypeTests.h"

#include <Arca/IsType.h>

namespace Arca
{
    const TypeHandleName Traits<IsTypeTestsFixture::Shard>::typeName =
        "IsTypeTestsShard";

    const TypeHandleName Traits<IsTypeTestsFixture::Relic>::typeName =
        "IsTypeTestsRelic";

    const TypeHandleName Traits<IsTypeTestsFixture::Curator>::typeName =
        "IsTypeTestsCurator";
}

SCENARIO_METHOD(IsTypeTestsFixture, "is type", "[IsType]")
{
    GIVEN("shard type handle name")
    {
        auto typeHandleName = TypeHandleFor<Shard>().name;

        WHEN("checking is shard")
        {
            THEN("returns true")
            {
                REQUIRE(IsType<Shard>(typeHandleName));
            }
        }

        WHEN("checking is relic")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Relic>(typeHandleName));
            }
        }

        WHEN("checking is curator")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Curator>(typeHandleName));
            }
        }
    }

    GIVEN("relic type handle name")
    {
        auto typeHandleName = TypeHandleFor<Relic>().name;

        WHEN("checking is shard")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Shard>(typeHandleName));
            }
        }

        WHEN("checking is relic")
        {
            THEN("returns true")
            {
                REQUIRE(IsType<Relic>(typeHandleName));
            }
        }

        WHEN("checking is curator")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Curator>(typeHandleName));
            }
        }
    }

    GIVEN("curator type handle name")
    {
        auto typeHandleName = TypeHandleFor<Curator>().name;

        WHEN("checking is shard")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Shard>(typeHandleName));
            }
        }

        WHEN("checking is relic")
        {
            THEN("returns false")
            {
                REQUIRE(!IsType<Relic>(typeHandleName));
            }
        }

        WHEN("checking is curator")
        {
            THEN("returns true")
            {
                REQUIRE(IsType<Curator>(typeHandleName));
            }
        }
    }
}