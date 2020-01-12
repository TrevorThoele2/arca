#include <catch.hpp>

#include "ShardTests.h"

#include <Arca/AsHandle.h>

SCENARIO_METHOD(ShardTestsFixture, "shard destruction")
{
    GIVEN("registered reliquary with regular shard")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Shard>()
            .Actualize();

        WHEN("creating open relic and non-const shard")
        {
            const auto relic = reliquary->Create<OpenRelic>();
            auto shard = relic->Create<Shard>();

            WHEN("destroying shard by either")
            {
                relic->Destroy<Either<Shard>>();

                THEN("shard is nullptr")
                {
                    REQUIRE(shard.Get() == nullptr);
                    REQUIRE(shard == nullptr);
                }

                THEN("shard is false")
                {
                    REQUIRE(!shard);
                }
            }
        }

        WHEN("creating open relic and const shard")
        {
            const auto relic = reliquary->Create<OpenRelic>();
            auto shard = relic->Create<const Shard>();

            WHEN("destroying shard by either")
            {
                relic->Destroy<Either<Shard>>();

                THEN("shard is nullptr")
                {
                    REQUIRE(shard.Get() == nullptr);
                    REQUIRE(shard == nullptr);
                }

                THEN("shard is false")
                {
                    REQUIRE(!shard);
                }
            }
        }
    }

    GIVEN("registered reliquary with three differentiable shards")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<DifferentiableShard<0>>()
            .Type<DifferentiableShard<1>>()
            .Type<DifferentiableShard<2>>()
            .Actualize();

        WHEN("creating open relic and three shards")
        {
            const auto relic = reliquary->Create<OpenRelic>();
            auto shard0 = relic->Create<DifferentiableShard<0>>();
            auto shard1 = relic->Create<DifferentiableShard<1>>();
            auto shard2 = relic->Create<DifferentiableShard<2>>();

            WHEN("destroying all shards by composite")
            {
                relic->Destroy<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>();

                THEN("shards are nullptr")
                {
                    REQUIRE(shard0.Get() == nullptr);
                    REQUIRE(shard0 == nullptr);
                    REQUIRE(shard1.Get() == nullptr);
                    REQUIRE(shard1 == nullptr);
                    REQUIRE(shard2.Get() == nullptr);
                    REQUIRE(shard2 == nullptr);
                }

                THEN("shards are false")
                {
                    REQUIRE(!shard0);
                    REQUIRE(!shard1);
                    REQUIRE(!shard2);
                }
            }

            WHEN("destroying two shards by composite")
            {
                relic->Destroy<All<DifferentiableShard<0>, DifferentiableShard<1>>>();

                THEN("two shards are nullptr")
                {
                    REQUIRE(shard0.Get() == nullptr);
                    REQUIRE(shard0 == nullptr);
                    REQUIRE(shard1.Get() == nullptr);
                    REQUIRE(shard1 == nullptr);
                }

                THEN("one shard exists")
                {
                    REQUIRE(shard2.Get() != nullptr);
                    REQUIRE(shard2 != nullptr);
                }

                THEN("two shards are false")
                {
                    REQUIRE(!shard0);
                    REQUIRE(!shard1);
                }

                THEN("one shard is true")
                {
                    REQUIRE(shard2);
                }
            }

            WHEN("finding all shards by composite")
            {
                auto tuple = *relic->Find<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>();

                THEN("tuple shards are equal to created")
                {
                    REQUIRE(shard0 == std::get<0>(tuple));
                    REQUIRE(shard1 == std::get<1>(tuple));
                    REQUIRE(shard2 == std::get<2>(tuple));
                }

                THEN("tuple shards are true")
                {
                    REQUIRE(std::get<0>(tuple));
                    REQUIRE(std::get<1>(tuple));
                    REQUIRE(std::get<2>(tuple));
                }
            }

            WHEN("finding two shards by composite")
            {
                auto tuple = *relic->Find<All<DifferentiableShard<1>, DifferentiableShard<2>>>();

                THEN("tuple shards are equal to created")
                {
                    REQUIRE(shard1 == std::get<0>(tuple));
                    REQUIRE(shard2 == std::get<1>(tuple));
                }

                THEN("tuple shards are true")
                {
                    REQUIRE(std::get<0>(tuple));
                    REQUIRE(std::get<1>(tuple));
                }
            }
        }
    }
}

SCENARIO_METHOD(ShardTestsFixture, "shard signals")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Shard>()
            .Actualize();

        auto genericCreatedSignals = reliquary->Batch<Created>();
        auto genericDestroyingSignals = reliquary->Batch<Destroying>();

        WHEN("creating open relic and non-const shard")
        {
            auto knownCreatedSignals = reliquary->Batch<CreatedKnown<Shard>>();
            auto knownDestroyingSignals = reliquary->Batch<DestroyingKnown<Shard>>();

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

            WHEN("destroying shard by either")
            {
                relic->Destroy<Either<Shard>>();

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

        WHEN("creating open relic and const shard")
        {
            auto knownCreatedSignals = reliquary->Batch<CreatedKnown<const Shard>>();
            auto knownDestroyingSignals = reliquary->Batch<DestroyingKnown<const Shard>>();

            const auto relic = reliquary->Create<OpenRelic>();
            auto shard = relic->Create<const Shard>();
            auto shardHandle = AsHandle<const Shard>(shard.ID(), *shard.Owner());

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
                relic->Destroy<const Shard>();

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

            WHEN("destroying shard by either")
            {
                relic->Destroy<Either<Shard>>();

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