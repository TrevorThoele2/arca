#include <catch.hpp>

#include "PtrTests.h"

#include <Arca/ReliquaryOrigin.h>

void PtrTestsFixture::TypedClosedRelic::PostConstruct(ShardTuple shards)
{
    shard = std::get<0>(shards);
}

void PtrTestsFixture::TypedOpenRelic::PostConstruct(ShardTuple shards)
{
    shard = std::get<0>(shards);
}

SCENARIO_METHOD(PtrTestsFixture, "LocalPtr resets to nullptr after underlying object destroyed", "[ptr]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Shard>()
            .Type<TypedClosedRelic>()
            .Type<TypedOpenRelic>()
            .Actualize();

        WHEN("creating open relic then destroying relic")
        {
            auto relic = reliquary->Create<OpenRelic>();

            reliquary->Destroy(*relic);

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
            auto relic = reliquary->Create<ClosedRelic>();

            reliquary->Destroy(*relic);

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
            auto relic = reliquary->Create<TypedClosedRelic>();

            reliquary->Destroy(*relic);

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
            auto relic = reliquary->Create<TypedOpenRelic>();

            reliquary->Destroy(*relic);

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

SCENARIO_METHOD(PtrTestsFixture, "MatrixPtr", "[ptr][matrix]")
{
    GIVEN("registered reliquary, open relic and three shards")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<DifferentiableShard<0>>()
            .Type<DifferentiableShard<1>>()
            .Type<DifferentiableShard<2>>()
            .Actualize();

        auto relic = reliquary->Create<OpenRelic>();
        auto shard0 = relic->Create<DifferentiableShard<0>>();
        auto shard1 = relic->Create<DifferentiableShard<1>>();
        auto shard2 = relic->Create<DifferentiableShard<2>>();

        WHEN("creating MatrixPtr out of all")
        {
            auto ptr = MatrixPtr<All<
                DifferentiableShard<0>,
                DifferentiableShard<1>,
                DifferentiableShard<2>>>
                (relic.ID(), *reliquary);

            THEN("dereferencing results in tuple of all, valid ptrs")
            {
                auto tuple = *ptr;
                auto ptr0 = std::get<0>(tuple);
                auto ptr1 = std::get<1>(tuple);
                auto ptr2 = std::get<2>(tuple);
                REQUIRE(ptr0);
                REQUIRE(ptr1);
                REQUIRE(ptr2);
                REQUIRE(shard0 == ptr0);
                REQUIRE(shard1 == ptr1);
                REQUIRE(shard2 == ptr2);
            }

            THEN("is true")
            {
                REQUIRE(ptr);
            }

            THEN("id is same as relic")
            {
                REQUIRE(ptr.ID() == relic.ID());
            }

            THEN("owner is same as relic")
            {
                REQUIRE(ptr.Owner() == relic.Owner());
            }
        }
    }
}