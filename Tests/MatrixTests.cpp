#include <catch.hpp>

#include "MatrixTests.h"

SCENARIO_METHOD(MatrixTestsFixture, "matrix signals")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Actualize();

        WHEN("batching up matrix formed and creating shard")
        {
            auto batch = reliquary->Batch<MatrixFormed<Either<Shard>>>();

            auto relic = reliquary->Create<OpenRelic>();
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

            auto relic = reliquary->Create<OpenRelic>();
            auto shard = relic->Create<Shard>();
            relic->Destroy<Shard>();

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

            auto relic = reliquary->Create<OpenRelic>();
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
                [createdShard, &createdShardSameAsSignaled](const MatrixDissolved<Either<Shard>>& signal)
                {
                    createdShardSameAsSignaled = createdShard == &*signal.index;
                });

            auto relic = reliquary->Create<OpenRelic>();
            auto shard = relic->Create<Shard>();
            createdShard = &*shard;
            relic->Destroy<Shard>();

            THEN("shard from execution is occupied")
            {
                REQUIRE(createdShardSameAsSignaled);
            }
        }
    }
}