#include <catch.hpp>

#include "ShardTests.h"
#include "SignalListener.h"

#include <Arca/AsHandle.h>

ShardTestsFixture::ShardConstructedFromMovedValue::ShardConstructedFromMovedValue(std::unique_ptr<int>&& myInt) :
    myInt(std::move(myInt))
{}

SCENARIO_METHOD(ShardTestsFixture, "shard destruction")
{
    GIVEN("registered reliquary with regular shard")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Actualize();

        WHEN("creating open relic and non-const shard")
        {
            const auto relic = reliquary->Do<Create<OpenRelic>>();
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
            const auto relic = reliquary->Do<Create<OpenRelic>>();
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
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Actualize();

        WHEN("creating open relic and three shards")
        {
            const auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard0 = relic->Create<DifferentiableShard<0>>();
            auto shard1 = relic->Create<DifferentiableShard<1>>();
            auto shard2 = relic->Create<DifferentiableShard<2>>();

            WHEN("destroying all shards by all")
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

            WHEN("destroying two shards by all")
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

            WHEN("finding all shards by all")
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

            WHEN("finding two shards by all")
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
            .Register<Shard>()
            .Actualize();

        auto genericCreatedSignals = SignalListener<Created>(*reliquary);
        auto genericDestroyingSignals = SignalListener<Destroying>(*reliquary);

        WHEN("creating open relic and non-const shard")
        {
            auto knownCreatedSignals = SignalListener<CreatedKnown<Shard>>(*reliquary);
            auto knownDestroyingSignals = SignalListener<DestroyingKnown<Shard>>(*reliquary);

            const auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<Shard>();
            auto shardHandle = AsHandle<Shard>(shard.ID(), *shard.Owner());

            THEN("generic signal is emitted for relic and shard")
            {
                REQUIRE(genericCreatedSignals.Executions().size() == 2);
                REQUIRE(std::any_of(
                    genericCreatedSignals.Executions().begin(),
                    genericCreatedSignals.Executions().end(),
                    [shardHandle](const Created& signal)
                    {
                        return signal.handle == shardHandle;
                    }));
            }

            THEN("signal is emitted for known shard")
            {
                REQUIRE(knownCreatedSignals.Executions().size() == 1);
                REQUIRE(knownCreatedSignals.Executions().begin()->reference == shard);
            }

            WHEN("destroying shard")
            {
                relic->Destroy<Shard>();

                THEN("signal is emitted for shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == shardHandle);
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->reference == shard);
                }
            }

            WHEN("destroying shard by either")
            {
                relic->Destroy<Either<Shard>>();

                THEN("signal is emitted for shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == shardHandle);
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->reference == shard);
                }
            }
        }

        WHEN("creating open relic and const shard")
        {
            auto knownCreatedSignals = SignalListener<CreatedKnown<const Shard>>(*reliquary);
            auto knownDestroyingSignals = SignalListener<DestroyingKnown<const Shard>>(*reliquary);

            const auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<const Shard>();
            auto shardHandle = AsHandle<const Shard>(shard.ID(), *shard.Owner());

            THEN("generic signal is emitted for relic and shard")
            {
                REQUIRE(genericCreatedSignals.Executions().size() == 2);
                REQUIRE(std::any_of(
                    genericCreatedSignals.Executions().begin(),
                    genericCreatedSignals.Executions().end(),
                    [shardHandle](const Created& signal)
                    {
                        return signal.handle == shardHandle;
                    }));
            }

            THEN("signal is emitted for known shard")
            {
                REQUIRE(knownCreatedSignals.Executions().size() == 1);
                REQUIRE(knownCreatedSignals.Executions().begin()->reference == shard);
            }

            WHEN("destroying shard")
            {
                relic->Destroy<const Shard>();

                THEN("signal is emitted for shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == shardHandle);
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->reference == shard);
                }
            }

            WHEN("destroying shard by either")
            {
                relic->Destroy<Either<Shard>>();

                THEN("signal is emitted for shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == shardHandle);
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->reference == shard);
                }
            }
        }
    }
}

SCENARIO_METHOD(ShardTestsFixture, "either signals", "[either][signal]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Actualize();

        auto createdSignals = SignalListener<MatrixFormed<Either<Shard>>>(*reliquary);
        auto destroyingSignals = SignalListener<MatrixDissolved<Either<Shard>>>(*reliquary);

        WHEN("creating open relic and non-const shard")
        {
            const auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<Shard>();
            auto shardHandle = AsHandle<Shard>(shard.ID(), *shard.Owner());

            THEN("signal is emitted for known shard")
            {
                REQUIRE(createdSignals.Executions().size() == 1);
                REQUIRE(createdSignals.Executions().begin()->index == shard);
            }

            WHEN("destroying shard")
            {
                relic->Destroy<Shard>();

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(destroyingSignals.Executions().size() == 1);
                    REQUIRE(destroyingSignals.Executions().begin()->index == shard);
                }
            }

            WHEN("destroying shard by either")
            {
                relic->Destroy<Either<Shard>>();

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(destroyingSignals.Executions().size() == 1);
                    REQUIRE(destroyingSignals.Executions().begin()->index == shard);
                }
            }
        }

        WHEN("creating open relic and const shard")
        {
            const auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<const Shard>();
            auto shardHandle = AsHandle<const Shard>(shard.ID(), *shard.Owner());

            THEN("signal is emitted for known shard")
            {
                REQUIRE(createdSignals.Executions().size() == 1);
                REQUIRE(createdSignals.Executions().begin()->index == shard);
            }

            WHEN("destroying shard")
            {
                relic->Destroy<const Shard>();

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(destroyingSignals.Executions().size() == 1);
                    REQUIRE(destroyingSignals.Executions().begin()->index == shard);
                }
            }

            WHEN("destroying shard by either")
            {
                relic->Destroy<Either<Shard>>();

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(destroyingSignals.Executions().size() == 1);
                    REQUIRE(destroyingSignals.Executions().begin()->index == shard);
                }
            }
        }
    }
}

SCENARIO_METHOD(ShardTestsFixture, "shard constructed from moved value", "[shard]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<ShardConstructedFromMovedValue>()
            .Actualize();

        WHEN("creating shard from moved value")
        {
            auto relic = reliquary->Do<Create<OpenRelic>>();

            const auto generatedInt = dataGeneration.Random<int>();
            auto backingInt = new int(generatedInt);
            auto myInt = std::unique_ptr<int>(backingInt);

            auto shard = relic->Create<ShardConstructedFromMovedValue>(std::move(myInt));

            THEN("has moved value")
            {
                REQUIRE(shard->myInt.get() == backingInt);
            }
        }
    }
}