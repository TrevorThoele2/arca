#include <catch.hpp>
using namespace std::string_literals;

#include "TypeTests.h"

#include <Inscription/Binary.h>

SCENARIO_METHOD(TypeTestsFixture, "type serialization", "[type][serialization]")
{
    GIVEN("relic type")
    {
        const auto savedType = TypeFor<TypedRelic>();

        WHEN("saving")
        {
            Inscription::Binary::ToFile(savedType, "Test.dat");

            THEN("loading gives same as saved")
            {
                Type loadedType;

                Inscription::Binary::FromFile(loadedType, "Test.dat");

                REQUIRE(loadedType.name == savedType.name);
                REQUIRE(loadedType.isConst == savedType.isConst);
            }
        }
    }

    GIVEN("shard type")
    {
        const auto savedType = TypeFor<Shard>();

        WHEN("saving")
        {
            Inscription::Binary::ToFile(savedType, "Test.dat");

            THEN("loading gives same as saved")
            {
                Type loadedType;

                Inscription::Binary::FromFile(loadedType, "Test.dat");

                REQUIRE(loadedType.name == savedType.name);
                REQUIRE(loadedType.isConst == savedType.isConst);
            }
        }
    }
}