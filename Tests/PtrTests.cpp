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