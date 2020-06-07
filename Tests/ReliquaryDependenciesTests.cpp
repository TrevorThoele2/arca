#include <catch.hpp>

#include "ReliquaryDependenciesTests.h"

#include <Arca/ReliquaryOrigin.h>
#include <Arca/ReliquaryDependencies.h>
#include <Arca/ReliquaryDependencyAlreadyAdded.h>

SCENARIO_METHOD(ReliquaryDependenciesTestsFixture, "adding dependencies", "[reliquarydependencies]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<ReliquaryDependencies>()
            .Actualize();

        WHEN("adding dependency")
        {
            const auto name = dataGeneration.Random<std::string>();

            reliquary->Do(AddReliquaryDependency{ name, reliquary.get() });

            THEN("can find with name")
            {
                auto found = reliquary->Do(FindReliquaryDependency{ name });
                REQUIRE(found->name == name);
                REQUIRE(found->reliquary == reliquary.get());
            }

            THEN("adding reliquary twice")
            {
                const auto otherName = dataGeneration.Random<std::string>();

                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(AddReliquaryDependency{ otherName, reliquary.get() }),
                    Arca::ReliquaryDependencyAlreadyAdded,
                    Catch::Matchers::Message(
                        "Reliquary dependency has already been added.")
                );
            }

            THEN("adding dependency with same name throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(AddReliquaryDependency{ name, reliquary.get() }),
                    Arca::ReliquaryDependencyAlreadyAdded,
                    Catch::Matchers::Message(
                        "Reliquary dependency name has already been added (" + name + ").")
                );
            }
        }
    }
}