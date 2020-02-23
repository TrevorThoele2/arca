#include <catch.hpp>

#include "PostulateTests.h"

PostulateTestsFixture::GlobalRelic::GlobalRelic(Init init, int integer, std::string string) :
    ClosedTypedRelic(init), integer(integer), string(string)
{}

SCENARIO_METHOD(PostulateTestsFixture, "default postulate", "[postulate]")
{
    GIVEN("default basic postulate")
    {
        auto postulate = Arca::Postulate<int>();

        WHEN("querying owner")
        {
            THEN("owner is nullptr")
            {
                REQUIRE(postulate.Owner() == nullptr);
            }
        }

        WHEN("converting to bool")
        {
            THEN("is considered false")
            {
                REQUIRE(!postulate);
            }
        }
    }

    GIVEN("default point postulate")
    {
        auto postulate = Arca::Postulate<int*>();

        WHEN("querying owner")
        {
            THEN("owner is nullptr")
            {
                REQUIRE(postulate.Owner() == nullptr);
            }
        }

        WHEN("converting to bool")
        {
            THEN("is considered false")
            {
                REQUIRE(!postulate);
            }
        }
    }
}

SCENARIO_METHOD(PostulateTestsFixture, "basic postulate", "[postulate]")
{
    GIVEN("registed reliquary")
    {
        auto integer = dataGeneration.Random<int>();
        auto string = dataGeneration.Random<std::string>();

        auto reliquary = Arca::ReliquaryOrigin()
            .Register<GlobalRelic>(integer, string)
            .Postulate<int>(
                [](Arca::Reliquary& reliquary)
                {
                    return Arca::Index<GlobalRelic>(reliquary)->integer;
                })
            .Postulate<std::string>(
                [](Arca::Reliquary& reliquary)
                {
                    return Arca::Index<GlobalRelic>(reliquary)->string;
                })
            .Actualize();

        WHEN("retrieving integer postulate")
        {
            Arca::Postulate<int> postulate(*reliquary);

            THEN("is correct value")
            {
                REQUIRE(*postulate == integer);
                REQUIRE(postulate.Get() == integer);
            }

            THEN("owner is correct")
            {
                REQUIRE(postulate.Owner() == reliquary.get());
            }

            THEN("is considered true")
            {
                REQUIRE(postulate);
            }
        }

        WHEN("retrieving string postulate")
        {
            Arca::Postulate<std::string> postulate(*reliquary);

            THEN("is correct value")
            {
                REQUIRE(*postulate == string);
                REQUIRE(postulate.Get() == string);
            }

            THEN("owner is correct")
            {
                REQUIRE(postulate.Owner() == reliquary.get());
            }

            THEN("is considered true")
            {
                REQUIRE(postulate);
            }
        }
    }
}

SCENARIO_METHOD(PostulateTestsFixture, "const basic postulate", "[postulate]")
{
    GIVEN("registed reliquary")
    {
        auto integer = dataGeneration.Random<int>();
        auto string = dataGeneration.Random<std::string>();

        auto reliquary = Arca::ReliquaryOrigin()
            .Register<GlobalRelic>(integer, string)
            .Postulate<const int>(
                [](Arca::Reliquary& reliquary)
                {
                    return Arca::Index<GlobalRelic>(reliquary)->integer;
                })
            .Postulate<const std::string>(
                [](Arca::Reliquary& reliquary)
                {
                    return Arca::Index<GlobalRelic>(reliquary)->string;
                })
                    .Actualize();

        WHEN("retrieving integer postulate")
        {
            Arca::Postulate<const int> postulate(*reliquary);

            THEN("is correct value")
            {
                REQUIRE(*postulate == integer);
                REQUIRE(postulate.Get() == integer);
            }

            THEN("owner is correct")
            {
                REQUIRE(postulate.Owner() == reliquary.get());
            }

            THEN("is considered true")
            {
                REQUIRE(postulate);
            }
        }

        WHEN("retrieving string postulate")
        {
            Arca::Postulate<const std::string> postulate(*reliquary);

            THEN("is correct value")
            {
                REQUIRE(*postulate == string);
                REQUIRE(postulate.Get() == string);
            }

            THEN("owner is correct")
            {
                REQUIRE(postulate.Owner() == reliquary.get());
            }

            THEN("is considered true")
            {
                REQUIRE(postulate);
            }
        }
    }
}

SCENARIO_METHOD(PostulateTestsFixture, "pointer postulate", "[postulate]")
{
    GIVEN("registed reliquary")
    {
        auto integer = dataGeneration.Random<int>();
        auto string = dataGeneration.Random<std::string>();

        auto reliquary = Arca::ReliquaryOrigin()
            .Register<GlobalRelic>(integer, string)
            .Postulate<const int*>(
                [](Arca::Reliquary& reliquary) -> const int*
                {
                    return &Arca::Index<GlobalRelic>(reliquary)->integer;
                })
            .Postulate<const std::string*>(
                [](Arca::Reliquary& reliquary) -> const std::string*
                {
                    return &Arca::Index<GlobalRelic>(reliquary)->string;
                })
            .Actualize();

        WHEN("retrieving integer pointer postulate")
        {
            Arca::Postulate<const int*> postulate(*reliquary);

            THEN("is correct value")
            {
                REQUIRE(*postulate == integer);
                REQUIRE(*postulate.Get() == integer);
            }

            THEN("owner is correct")
            {
                REQUIRE(postulate.Owner() == reliquary.get());
            }

            THEN("is considered true")
            {
                REQUIRE(postulate);
            }
        }

        WHEN("retrieving string postulate")
        {
            Arca::Postulate<const std::string*> postulate(*reliquary);

            THEN("is correct value")
            {
                REQUIRE(*postulate == string);
                REQUIRE(*postulate.Get() == string);
            }

            THEN("owner is correct")
            {
                REQUIRE(postulate.Owner() == reliquary.get());
            }

            THEN("is considered true")
            {
                REQUIRE(postulate);
            }
        }
    }
}