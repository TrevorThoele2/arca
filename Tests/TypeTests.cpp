#include <catch.hpp>
using namespace std::string_literals;

#include "TypeTests.h"

TypeTestsFixture::TypedRelic::TypedRelic(Init init) : ClosedTypedRelic(init)
{}

SCENARIO_METHOD(TypeTestsFixture, "type serialization", "[type][serialization]")
{
    GIVEN("relic type")
    {
        const auto savedType = TypeFor<TypedRelic>();

        WHEN("saving")
        {
            {
                auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
                outputArchive(savedType);
            }

            THEN("loading gives same as saved")
            {
                Type loadedType;

                {
                    auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                    inputArchive(loadedType);
                }

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
            {
                auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
                outputArchive(savedType);
            }

            THEN("loading gives same as saved")
            {
                Type loadedType;

                {
                    auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                    inputArchive(loadedType);
                }

                REQUIRE(loadedType.name == savedType.name);
                REQUIRE(loadedType.isConst == savedType.isConst);
            }
        }
    }
}