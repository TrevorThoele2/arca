#include <catch.hpp>

#include "ChildRelicBatchTests.h"

#include <Arca/ReliquaryOrigin.h>
#include <Arca/ExtractShards.h>

ChildRelicBatchTestsFixture::BasicTypedRelic::BasicTypedRelic(
    const ::Inscription::BinaryTableData<BasicTypedRelic>& data)
{}

void ChildRelicBatchTestsFixture::BasicTypedRelic::InitializeImplementation()
{
    using Shards = ShardsFor<BasicTypedRelic>;
    auto tuple = ExtractShards<Shards>(ID(), Owner());
    basicShard = std::get<0>(tuple);
}

namespace Arca
{
    const TypeHandle ShardTraits<ChildRelicBatchTestsFixture::BasicShard>::typeHandle =
        "ChildRelicTestsBasicShard";

    const TypeHandle RelicTraits<ChildRelicBatchTestsFixture::BasicTypedRelic>::typeHandle =
        "ChildRelicTestsBasicTypedRelic";
}

SCENARIO_METHOD(ChildRelicBatchTestsFixture, "default child relic batch")
{
    GIVEN("no types registered to reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Actualize();

        WHEN("retrieving child relic batch")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.ChildBatch<BasicTypedRelic>(0),
                    NotRegistered,
                    Catch::Matchers::Message("The relic (ChildRelicTestsBasicTypedRelic) was not registered.")
                );
            }
        }
    }

    GIVEN("relic registered to reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<BasicTypedRelic>()
            .Actualize();

        WHEN("retrieving child relic batch")
        {
            auto batch = reliquary.ChildBatch<BasicTypedRelic>(0);

            THEN("batch is empty")
            {
                REQUIRE(batch.IsEmpty());
                REQUIRE(batch.Size() == 0);
                REQUIRE(batch.begin() == batch.end());
            }
        }
    }
}

SCENARIO_METHOD(ChildRelicBatchTestsFixture, "child relic batch")
{
    GIVEN("parented dynamic relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Relic<BasicTypedRelic>()
            .Actualize();

        const auto parent = reliquary.Create<BasicTypedRelic>();
        const auto child = reliquary.Create<BasicTypedRelic>();
        reliquary.ParentRelic(parent->ID(), child->ID());

        WHEN("retrieving child relic batch")
        {
            auto batch = reliquary.ChildBatch<BasicTypedRelic>(parent->ID());

            THEN("batch is occupied")
            {
                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
                REQUIRE(&*batch.begin() == child);
            }

            WHEN("destroying typed child")
            {
                reliquary.Destroy(*child);

                THEN("batch is empty")
                {
                    REQUIRE(batch.IsEmpty());
                    REQUIRE(batch.Size() == 0);
                    REQUIRE(batch.begin() == batch.end());
                }
            }

            WHEN("destroying child id")
            {
                reliquary.DestroyRelic(child->ID());

                THEN("batch is empty")
                {
                    REQUIRE(batch.IsEmpty());
                    REQUIRE(batch.Size() == 0);
                    REQUIRE(batch.begin() == batch.end());
                }
            }

            WHEN("destroying dynamic child")
            {
                auto dynamicChild = reliquary.FindRelic(child->ID());
                reliquary.Destroy(*dynamicChild);

                THEN("batch is empty")
                {
                    REQUIRE(batch.IsEmpty());
                    REQUIRE(batch.Size() == 0);
                    REQUIRE(batch.begin() == batch.end());
                }
            }
        }
    }

    GIVEN("child relic batch")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Relic<BasicTypedRelic>()
            .Actualize();

        auto batch = reliquary.ChildBatch<BasicTypedRelic>(1);

        WHEN("creating and parenting relics")
        {
            const auto parent = reliquary.Create<BasicTypedRelic>();
            const auto child = reliquary.Create<BasicTypedRelic>();
            reliquary.ParentRelic(parent->ID(), child->ID());

            THEN("batch is occupied")
            {
                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
                REQUIRE(&*batch.begin() == child);
            }

            WHEN("destroying typed child")
            {
                reliquary.Destroy(*child);

                THEN("batch is empty")
                {
                    REQUIRE(batch.IsEmpty());
                    REQUIRE(batch.Size() == 0);
                    REQUIRE(batch.begin() == batch.end());
                }
            }

            WHEN("destroying child id")
            {
                reliquary.DestroyRelic(child->ID());

                THEN("batch is empty")
                {
                    REQUIRE(batch.IsEmpty());
                    REQUIRE(batch.Size() == 0);
                    REQUIRE(batch.begin() == batch.end());
                }
            }

            WHEN("destroying dynamic child")
            {
                auto dynamicChild = reliquary.FindRelic(child->ID());
                reliquary.Destroy(*dynamicChild);

                THEN("batch is empty")
                {
                    REQUIRE(batch.IsEmpty());
                    REQUIRE(batch.Size() == 0);
                    REQUIRE(batch.begin() == batch.end());
                }
            }
        }
    }
}