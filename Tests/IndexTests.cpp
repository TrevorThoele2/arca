#include <catch.hpp>

#include "IndexTests.h"

#include <Arca/ReliquaryOrigin.h>

IndexTestsFixture::TypedClosedRelic::TypedClosedRelic(Init init) :
    ClosedTypedRelic(init)
{
    shard = FindOrCreate<Shard>();
}

IndexTestsFixture::TypedOpenRelic::TypedOpenRelic(Init init) :
    OpenTypedRelic(init)
{
    shard = FindOrCreate<Shard>();
}

IndexTestsFixture::GlobalRelic::GlobalRelic(Init init) :
    ClosedTypedRelic(init)
{
    shard = FindOrCreate<Shard>();
}

SCENARIO_METHOD(IndexTestsFixture, "Index resets to nullptr after underlying object destroyed", "[index]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Register<TypedClosedRelic>()
            .Register<TypedOpenRelic>()
            .Actualize();

        WHEN("creating open relic then destroying relic")
        {
            auto relic = reliquary->Do<Create<OpenRelic>>();

            reliquary->Destroy(relic);

            THEN("relic is nullptr")
            {
                REQUIRE(relic.Get() == nullptr);
            }

            THEN("ID is 1")
            {
                REQUIRE(relic.ID() == 1);
            }

            THEN("owner is reliquary")
            {
                REQUIRE(relic.Owner() == reliquary.get());
            }
        }

        WHEN("creating closed relic then destroying relic")
        {
            auto relic = reliquary->Do<Create<ClosedRelic>>();

            reliquary->Destroy(relic);

            THEN("relic is nullptr")
            {
                REQUIRE(relic.Get() == nullptr);
            }

            THEN("ID is 1")
            {
                REQUIRE(relic.ID() == 1);
            }

            THEN("owner is reliquary")
            {
                REQUIRE(relic.Owner() == reliquary.get());
            }
        }

        WHEN("creating typed closed relic then destroying relic")
        {
            auto relic = reliquary->Do<Create<TypedClosedRelic>>();

            reliquary->Destroy(relic);

            THEN("relic is nullptr")
            {
                REQUIRE(relic.Get() == nullptr);
            }

            THEN("ID is 1")
            {
                REQUIRE(relic.ID() == 1);
            }

            THEN("owner is reliquary")
            {
                REQUIRE(relic.Owner() == reliquary.get());
            }
        }

        WHEN("creating typed open relic then destroying relic")
        {
            auto relic = reliquary->Do<Create<TypedOpenRelic>>();

            reliquary->Destroy(relic);

            THEN("relic is nullptr")
            {
                REQUIRE(relic.Get() == nullptr);
            }

            THEN("ID is 1")
            {
                REQUIRE(relic.ID() == 1);
            }

            THEN("owner is reliquary")
            {
                REQUIRE(relic.Owner() == reliquary.get());
            }
        }
    }
}

SCENARIO_METHOD(IndexTestsFixture, "Matrix Index", "[index][matrix]")
{
    GIVEN("registered reliquary, open relic and three shards")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Actualize();

        auto relic = reliquary->Do<Create<OpenRelic>>();
        auto shard0 = relic->Create<DifferentiableShard<0>>();
        auto shard1 = relic->Create<DifferentiableShard<1>>();
        auto shard2 = relic->Create<DifferentiableShard<2>>();

        WHEN("creating Index out of all")
        {
            auto index = MatrixIndex<All<
                DifferentiableShard<0>,
                DifferentiableShard<1>,
                DifferentiableShard<2>>>
                (relic.ID(), *reliquary);

            THEN("dereferencing results in tuple of all, valid indexs")
            {
                auto tuple = *index;
                auto index0 = std::get<0>(tuple);
                auto index1 = std::get<1>(tuple);
                auto index2 = std::get<2>(tuple);
                REQUIRE(index0);
                REQUIRE(index1);
                REQUIRE(index2);
                REQUIRE(shard0 == index0);
                REQUIRE(shard1 == index1);
                REQUIRE(shard2 == index2);
            }

            THEN("is true")
            {
                REQUIRE(index);
            }

            THEN("id is same as relic")
            {
                REQUIRE(index.ID() == relic.ID());
            }

            THEN("owner is same as relic")
            {
                REQUIRE(index.Owner() == relic.Owner());
            }
        }
    }
}