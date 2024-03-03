#include <catch.hpp>

#include "MatrixTests.h"

SCENARIO_METHOD(MatrixTestsFixture, "matrix signals", "[matrix][signal]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Actualize();

        WHEN("batching up matrix formed and creating shard")
        {
            auto batch = reliquary->Batch<MatrixFormed<Either<Shard>>>();

            auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<Shard>();

            THEN("batch contains entry")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
                REQUIRE(&*shard == &*batch.begin()->index);
            }
        }

        WHEN("batching up matrix dissolved and destroying shard")
        {
            auto batch = reliquary->Batch<MatrixDissolved<Either<Shard>>>();

            auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<Shard>();
            relic->Destroy<Shard>();

            THEN("batch contains entry")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
                REQUIRE(&*shard == &*batch.begin()->index);
            }
        }

        WHEN("batching up matrix dissolved and destroying relic")
        {
            auto batch = reliquary->Batch<MatrixDissolved<Either<Shard>>>();

            auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<Shard>();
            reliquary->Do<Destroy<OpenRelic>>(relic);

            THEN("batch contains entry")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
                REQUIRE(&*shard == &*batch.begin()->index);
            }
        }

        WHEN("executing on matrix formed and creating shard")
        {
            const Shard* shardFromExecution = nullptr;

            reliquary->ExecuteOn<MatrixFormed<Either<Shard>>>(
                [&shardFromExecution](const MatrixFormed<Either<Shard>>& signal)
                {
                    shardFromExecution = signal.index.Get();
                });

            auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<Shard>();

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

            reliquary->ExecuteOn<MatrixDissolved<Either<Shard>>>(
                [&createdShard, &createdShardSameAsSignaled](const MatrixDissolved<Either<Shard>>& signal)
                {
                    createdShardSameAsSignaled = createdShard == &*signal.index;
                });

            auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<Shard>();
            createdShard = &*shard;
            relic->Destroy<Shard>();

            THEN("shard from execution is occupied")
            {
                REQUIRE(createdShardSameAsSignaled);
            }
        }

        WHEN("executing on matrix dissolved and destroying relic")
        {
            const Shard* createdShard = nullptr;
            auto createdShardSameAsSignaled = false;

            reliquary->ExecuteOn<MatrixDissolved<Either<Shard>>>(
                [&createdShard, &createdShardSameAsSignaled](const MatrixDissolved<Either<Shard>>& signal)
                {
                    createdShardSameAsSignaled = createdShard == &*signal.index;
                });

            auto relic = reliquary->Do<Create<OpenRelic>>();
            auto shard = relic->Create<Shard>();
            createdShard = &*shard;
            reliquary->Do<Destroy<OpenRelic>>(relic);

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
            .Register<Shard>()
            .Register<OtherShard>()
            .Actualize();

        WHEN("batching up other matrix formed, creating other shard then shard")
        {
            auto batch = reliquary->Batch<MatrixFormed<Either<OtherShard>>>();

            auto relic = reliquary->Do<Create<OpenRelic>>();
            relic->Create<OtherShard>();
            relic->Create<Shard>();

            THEN("batch has size of 1")
            {
                REQUIRE(batch.Size() == 1);
            }
        }

        WHEN("batching up other matrix dissolved, creating other shard, then destroying shard")
        {
            auto batch = reliquary->Batch<MatrixDissolved<Either<OtherShard>>>();

            auto relic = reliquary->Do<Create<OpenRelic>>();
            relic->Create<Shard>();
            relic->Create<OtherShard>();
            relic->Destroy<Shard>();
            relic->Destroy<OtherShard>();

            THEN("batch has size of 1")
            {
                REQUIRE(batch.Size() == 1);
            }
        }

        WHEN("batching up other matrix dissolved, creating other shard, then destroying relic")
        {
            auto batch = reliquary->Batch<MatrixDissolved<Either<OtherShard>>>();

            auto relic = reliquary->Do<Create<OpenRelic>>();
            relic->Create<Shard>();
            relic->Create<OtherShard>();
            reliquary->Do<Destroy<OpenRelic>>(relic);

            THEN("batch has size of 1")
            {
                REQUIRE(batch.Size() == 1);
            }
        }

        WHEN("executing on other matrix formed, creating other shard then shard")
        {
            auto encounterCount = 0;

            reliquary->ExecuteOn<MatrixFormed<Either<OtherShard>>>(
                [&encounterCount](const MatrixFormed<Either<OtherShard>>& signal)
                {
                    ++encounterCount;
                });

            auto relic = reliquary->Do<Create<OpenRelic>>();
            relic->Create<OtherShard>();
            relic->Create<Shard>();

            THEN("encounter count is 1")
            {
                REQUIRE(encounterCount == 1);
            }
        }

        WHEN("executing on other matrix dissolved, creating other shard, then destroying shard")
        {
            auto encounterCount = 0;

            reliquary->ExecuteOn<MatrixDissolved<Either<OtherShard>>>(
                [&encounterCount](const MatrixDissolved<Either<OtherShard>>& signal)
                {
                    ++encounterCount;
                });

            auto relic = reliquary->Do<Create<OpenRelic>>();
            relic->Create<OtherShard>();
            relic->Create<Shard>();
            relic->Destroy<Shard>();
            relic->Destroy<OtherShard>();

            THEN("encounter count is 1")
            {
                REQUIRE(encounterCount == 1);
            }
        }

        WHEN("executing on other matrix dissolved, creating other shard, then destroying relic")
        {
            auto encounterCount = 0;

            reliquary->ExecuteOn<MatrixDissolved<Either<OtherShard>>>(
                [&encounterCount](const MatrixDissolved<Either<OtherShard>>& signal)
                {
                    ++encounterCount;
                });

            auto relic = reliquary->Do<Create<OpenRelic>>();
            relic->Create<OtherShard>();
            relic->Create<Shard>();
            reliquary->Do<Destroy<OpenRelic>>(relic);

            THEN("encounter count is 1")
            {
                REQUIRE(encounterCount == 1);
            }
        }
    }
}