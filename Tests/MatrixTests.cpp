#include <catch.hpp>

#include "MatrixTests.h"
#include <Arca/OpenRelic.h>
#include <Arca/Either.h>
#include <Arca/Create.h>
#include <Arca/Destroy.h>

SCENARIO_METHOD(MatrixTestsFixture, "matrix signals", "[matrix][signal]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<Shard>()
            .Actualize();

        WHEN("executing on matrix formed and creating shard")
        {
            const Shard* shardFromExecution = nullptr;

            reliquary->On<MatrixFormed<Either<Shard>>>(
                [&shardFromExecution](const MatrixFormed<Either<Shard>>& signal)
                {
                    shardFromExecution = signal.index.Get();
                });

            auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Arca::Create<Shard>(relic.ID()));

            THEN("shard from execution is occupied")
            {
                REQUIRE(shardFromExecution);
                REQUIRE(&*shard == shardFromExecution);
            }
        }

        WHEN("executing on matrix dissolved and destroying shard")
        {
            const Shard* createdShard = nullptr;
            auto createdShardSameAsSignaled = false;

            reliquary->On<MatrixDissolved<Either<Shard>>>(
                [&createdShard, &createdShardSameAsSignaled](const MatrixDissolved<Either<Shard>>& signal)
                {
                    createdShardSameAsSignaled = createdShard == &*signal.index;
                });

            auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Arca::Create<Shard>(relic.ID()));
            createdShard = &*shard;
            reliquary->Do(Arca::Destroy<Shard>(relic.ID()));

            THEN("shard from execution is occupied")
            {
                REQUIRE(createdShardSameAsSignaled);
            }
        }

        WHEN("executing on matrix dissolved and destroying relic")
        {
            const Shard* createdShard = nullptr;
            auto createdShardSameAsSignaled = false;

            reliquary->On<MatrixDissolved<Either<Shard>>>(
                [&createdShard, &createdShardSameAsSignaled](const MatrixDissolved<Either<Shard>>& signal)
                {
                    createdShardSameAsSignaled = createdShard == &*signal.index;
                });

            auto relic = reliquary->Do(Create<OpenRelic>());
            auto shard = reliquary->Do(Arca::Create<Shard>(relic.ID()));
            createdShard = &*shard;
            reliquary->Do(Destroy<OpenRelic>{relic});

            THEN("shard from execution is occupied")
            {
                REQUIRE(createdShardSameAsSignaled);
            }
        }
    }
}

SCENARIO_METHOD(MatrixTestsFixture, "matrix signals not executed", "[matrix][signal]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<Shard>()
            .Register<OtherShard>()
            .Actualize();

        WHEN("executing on other matrix formed, creating other shard then shard")
        {
            auto encounterCount = 0;

            reliquary->On<MatrixFormed<Either<OtherShard>>>(
                [&encounterCount](const MatrixFormed<Either<OtherShard>>&)
                {
                    ++encounterCount;
                });

            const auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Arca::Create<OtherShard>(relic.ID()));
            reliquary->Do(Arca::Create<Shard>(relic.ID()));

            THEN("encounter count is 1")
            {
                REQUIRE(encounterCount == 1);
            }
        }

        WHEN("executing on other matrix dissolved, creating other shard, then destroying shard")
        {
            auto encounterCount = 0;

            reliquary->On<MatrixDissolved<Either<OtherShard>>>(
                [&encounterCount](const MatrixDissolved<Either<OtherShard>>&)
                {
                    ++encounterCount;
                });

            const auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Arca::Create<OtherShard>(relic.ID()));
            reliquary->Do(Arca::Create<Shard>(relic.ID()));
            reliquary->Do(Arca::Destroy<Shard>(relic.ID()));
            reliquary->Do(Arca::Destroy<OtherShard>(relic.ID()));

            THEN("encounter count is 1")
            {
                REQUIRE(encounterCount == 1);
            }
        }

        WHEN("executing on other matrix dissolved, creating other shard, then destroying relic")
        {
            auto encounterCount = 0;

            reliquary->On<MatrixDissolved<Either<OtherShard>>>(
                [&encounterCount](const MatrixDissolved<Either<OtherShard>>&)
                {
                    ++encounterCount;
                });

            const auto relic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Arca::Create<OtherShard>(relic.ID()));
            reliquary->Do(Arca::Create<Shard>(relic.ID()));
            reliquary->Do(Destroy<OpenRelic>{relic});

            THEN("encounter count is 1")
            {
                REQUIRE(encounterCount == 1);
            }
        }
    }
}