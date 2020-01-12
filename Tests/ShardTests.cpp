#include <catch.hpp>

#include "ShardTests.h"

#include <Arca/AsHandle.h>

SCENARIO_METHOD(ShardTestsFixture, "shard signals")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Shard>()
            .Actualize();

        auto genericCreatedSignals = reliquary->Batch<Created>();
        auto genericDestroyingSignals = reliquary->Batch<Destroying>();

        auto knownCreatedSignals = reliquary->Batch<CreatedKnown<Shard>>();
        auto knownDestroyingSignals = reliquary->Batch<DestroyingKnown<Shard>>();

        WHEN("creating open relic and shard")
        {
            const auto relic = reliquary->Create<OpenRelic>();
            auto shard = relic->Create<Shard>();
            auto shardHandle = AsHandle<Shard>(shard.ID(), *shard.Owner());

            THEN("generic signal is emitted for relic and shard")
            {
                REQUIRE(genericCreatedSignals.Size() == 2);
                REQUIRE(std::any_of(
                    genericCreatedSignals.begin(),
                    genericCreatedSignals.end(),
                    [shardHandle](const Created& signal)
                    {
                        return signal.handle == shardHandle;
                    }));
            }

            THEN("signal is emitted for known shard")
            {
                REQUIRE(knownCreatedSignals.Size() == 1);
                REQUIRE(knownCreatedSignals.begin()->ptr == shard);
            }

            WHEN("destroying shard")
            {
                relic->Destroy<Shard>();

                THEN("signal is emitted for shard")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 1);
                    REQUIRE(genericDestroyingSignals.begin()->handle == shardHandle);
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->ptr == shard);
                }
            }
        }
    }
}