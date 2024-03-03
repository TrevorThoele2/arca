#include <catch.hpp>

#include "ShardTests.h"
#include "SignalListener.h"

#include <Arca/OpenRelic.h>
#include <Arca/AsHandle.h>

#include "BasicShard.h"
#include "DifferentiableShard.h"

ShardTestsFixture::RelicConstructingShard::RelicConstructingShard(RelicInit init) :
    shard(init.Create<BasicShard>())
{}

ShardTestsFixture::ShardConstructedFromMovedValue::ShardConstructedFromMovedValue(std::unique_ptr<int>&& myInt) :
    myInt(std::move(myInt))
{}

SCENARIO_METHOD(ShardTestsFixture, "shard destruction")
{
    GIVEN("registered reliquary with regular shard")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<RelicConstructingShard>()
            .Register<BasicShard>()
            .Register<DifferentiableShard<0>>()
            .Actualize();

        WHEN("creating open relic and non-const shard")
        {
            const auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Create<BasicShard>(relic.ID()));

            WHEN("destroying shard by either")
            {
                reliquary->Do(Destroy<Either<BasicShard>>(relic.ID()));

                THEN("shard is destroyed")
                {
                    REQUIRE(shard.Get() == nullptr);
                    REQUIRE(shard == nullptr);
                    REQUIRE(!shard);
                }
            }
        }

        WHEN("creating open relic and const shard")
        {
            const auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Create<const BasicShard>(relic.ID()));

            WHEN("destroying shard by either")
            {
                reliquary->Do(Destroy<Either<BasicShard>>(relic.ID()));

                THEN("shard is destroyed")
                {
                    REQUIRE(shard.Get() == nullptr);
                    REQUIRE(shard == nullptr);
                    REQUIRE(!shard);
                }
            }
        }

        WHEN("creating relic with creating shard")
        {
            const auto relic = reliquary->Do(Create<RelicConstructingShard>());

            WHEN("destroying shard")
            {
                THEN("throws error")
                {
                    REQUIRE_THROWS_MATCHES(
                        reliquary->Do(Destroy<BasicShard>(relic.ID())),
                        Arca::CannotDestroy,
                        ::Catch::Matchers::Message("The shard (" + Chroma::ToString(TypeFor<BasicShard>()) + ") cannot be destroyed."));
                }
            }

            WHEN("clearing relics")
            {
                reliquary->Do(Clear(TypeFor<RelicConstructingShard>()));

                THEN("shard is destroyed")
                {
                    auto shard = Arca::Index<BasicShard>(relic.ID(), *reliquary);
                    REQUIRE(shard == nullptr);
                    REQUIRE(!shard);
                }
            }

            WHEN("creating optional shard")
            {
                reliquary->Do(Create<DifferentiableShard<0>>(relic.ID()));

                WHEN("destroying optional shard")
                {
                    reliquary->Do(Destroy<DifferentiableShard<0>>(relic.ID()));

                    THEN("shard is destroyed")
                    {
                        auto shard = Arca::Index<DifferentiableShard<0>>(relic.ID(), *reliquary);
                        REQUIRE(shard == nullptr);
                        REQUIRE(!shard);
                    }
                }
            }
        }

        WHEN("creating open relic with relic structure")
        {
            const auto relic = reliquary->Do(CreateWith<OpenRelic>(RelicStructure{ TypeFor<BasicShard>() }));

            WHEN("destroying shard")
            {
                THEN("throws error")
                {
                    REQUIRE_THROWS_MATCHES(
                        reliquary->Do(Destroy<BasicShard>(relic.ID())),
                        Arca::CannotDestroy,
                        ::Catch::Matchers::Message("The shard (" + Chroma::ToString(TypeFor<BasicShard>()) + ") cannot be destroyed."));
                }
            }

            WHEN("clearing relics")
            {
                reliquary->Do(Clear(TypeFor<OpenRelic>()));

                THEN("shard is destroyed")
                {
                    auto shard = Arca::Index<BasicShard>(relic.ID(), *reliquary);
                    REQUIRE(shard == nullptr);
                    REQUIRE(!shard);
                }
            }

            WHEN("creating optional shard")
            {
                reliquary->Do(Create<DifferentiableShard<0>>(relic.ID()));

                WHEN("destroying optional shard")
                {
                    reliquary->Do(Destroy<DifferentiableShard<0>>(relic.ID()));

                    THEN("shard is destroyed")
                    {
                        auto shard = Arca::Index<DifferentiableShard<0>>(relic.ID(), *reliquary);
                        REQUIRE(shard == nullptr);
                        REQUIRE(!shard);
                    }
                }
            }
        }
    }

    GIVEN("registered reliquary with three differentiable shards")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Actualize();

        WHEN("creating open relic and three shards")
        {
            const auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard0 = reliquary->Do(Create<DifferentiableShard<0>>(relic.ID()));
            auto shard1 = reliquary->Do(Create<DifferentiableShard<1>>(relic.ID()));
            auto shard2 = reliquary->Do(Create<DifferentiableShard<2>>(relic.ID()));

            WHEN("destroying all shards by all")
            {
                reliquary->Do(
                    Destroy<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(relic.ID()));

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
                reliquary->Do(Destroy<All<DifferentiableShard<0>, DifferentiableShard<1>>>(relic.ID()));

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
                auto tuple =
                    *Arca::Index<All<DifferentiableShard<0>, DifferentiableShard<1>, DifferentiableShard<2>>>(
                        relic.ID(), *reliquary);

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
                auto tuple = *Arca::Index<All<DifferentiableShard<1>, DifferentiableShard<2>>>(
                    relic.ID(), *reliquary);

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
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto genericCreatedSignals = SignalListener<Created>(*reliquary);
        auto genericDestroyingSignals = SignalListener<Destroying>(*reliquary);

        WHEN("creating open relic and non-const shard")
        {
            auto knownCreatedSignals = SignalListener<CreatedKnown<BasicShard>>(*reliquary);
            auto knownDestroyingSignals = SignalListener<DestroyingKnown<BasicShard>>(*reliquary);

            const auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Create<BasicShard>(relic.ID()));
            auto shardHandle = AsHandle<BasicShard>(shard.ID());

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
                REQUIRE(knownCreatedSignals.Executions().begin()->index == shard);
            }

            WHEN("destroying shard")
            {
                reliquary->Do(Destroy<BasicShard>(relic.ID()));

                THEN("signal is emitted for shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == shardHandle);
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == shard);
                }
            }

            WHEN("destroying shard by either")
            {
                reliquary->Do(Destroy<Either<BasicShard>>(relic.ID()));

                THEN("signal is emitted for shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == shardHandle);
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == shard);
                }
            }
        }

        WHEN("creating open relic and const shard")
        {
            auto knownCreatedSignals = SignalListener<CreatedKnown<const BasicShard>>(*reliquary);
            auto knownDestroyingSignals = SignalListener<DestroyingKnown<const BasicShard>>(*reliquary);

            const auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Create<const BasicShard>(relic.ID()));
            auto shardHandle = AsHandle<const BasicShard>(shard.ID());

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
                REQUIRE(knownCreatedSignals.Executions().begin()->index == shard);
            }

            WHEN("destroying shard")
            {
                reliquary->Do(Destroy<const BasicShard>(relic.ID()));

                THEN("signal is emitted for shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == shardHandle);
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == shard);
                }
            }

            WHEN("destroying shard by either")
            {
                reliquary->Do(Destroy<Either<BasicShard>>(relic.ID()));

                THEN("signal is emitted for shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == shardHandle);
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == shard);
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
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto createdSignals = SignalListener<MatrixFormed<Either<BasicShard>>>(*reliquary);
        auto destroyingSignals = SignalListener<MatrixDissolved<Either<BasicShard>>>(*reliquary);

        WHEN("creating open relic and non-const shard")
        {
            const auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Create<BasicShard>(relic.ID()));
            auto shardHandle = AsHandle<BasicShard>(shard.ID());

            THEN("signal is emitted for known shard")
            {
                REQUIRE(createdSignals.Executions().size() == 1);
                REQUIRE(createdSignals.Executions().begin()->index == shard);
            }

            WHEN("destroying shard")
            {
                reliquary->Do(Destroy<BasicShard>(relic.ID()));

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(destroyingSignals.Executions().size() == 1);
                    REQUIRE(destroyingSignals.Executions().begin()->index == shard);
                }
            }

            WHEN("destroying shard by either")
            {
                reliquary->Do(Destroy<Either<BasicShard>>(relic.ID()));

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(destroyingSignals.Executions().size() == 1);
                    REQUIRE(destroyingSignals.Executions().begin()->index == shard);
                }
            }
        }

        WHEN("creating open relic and const shard")
        {
            const auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Create<const BasicShard>(relic.ID()));
            auto shardHandle = AsHandle<const BasicShard>(shard.ID());

            THEN("signal is emitted for known shard")
            {
                REQUIRE(createdSignals.Executions().size() == 1);
                REQUIRE(createdSignals.Executions().begin()->index == shard);
            }

            WHEN("destroying shard")
            {
                reliquary->Do(Destroy<const BasicShard>(relic.ID()));

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(destroyingSignals.Executions().size() == 1);
                    REQUIRE(destroyingSignals.Executions().begin()->index == shard);
                }
            }

            WHEN("destroying shard by either")
            {
                reliquary->Do(Destroy<Either<BasicShard>>(relic.ID()));

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
            .Register<OpenRelic>()
            .Register<ShardConstructedFromMovedValue>()
            .Actualize();

        WHEN("creating shard from moved value")
        {
            const auto relic = reliquary->Do(Create<OpenRelic>());

            const auto generatedInt = dataGeneration.Random<int>();
            auto backingInt = new int(generatedInt);
            auto myInt = std::unique_ptr<int>(backingInt);

            auto shard = reliquary->Do(Create<ShardConstructedFromMovedValue>(relic.ID(), std::move(myInt)));

            THEN("has moved value")
            {
                REQUIRE(shard->myInt.get() == backingInt);
            }
        }
    }
}