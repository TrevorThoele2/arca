#include <catch.hpp>

#include "ChildRelicBatchTests.h"

#include <Arca/ReliquaryOrigin.h>
#include <Arca/ExtractShards.h>

ChildRelicBatchTestsFixture::BasicTypedRelic::BasicTypedRelic(
    const ::Inscription::BinaryTableData<BasicTypedRelic> & data)
{}

void ChildRelicBatchTestsFixture::BasicTypedRelic::Initialize(Reliquary &reliquary)
{
    auto tuple = ExtractShards<Shards>(ID(), reliquary);
    basicShard = std::get<0>(tuple);
}

RelicStructure ChildRelicBatchTestsFixture::BasicTypedRelic::Structure() const
{
    return StructureFrom<Shards>();
}

namespace Arca
{
    const TypeHandle ShardTraits<ChildRelicBatchTestsFixture::BasicShard>::typeHandle =
        "ChildRelicTestsBasicShard";

    const TypeHandle RelicTraits<ChildRelicBatchTestsFixture::BasicTypedRelic>::typeHandle =
        "ChildRelicTestsBasicTypedRelic";
}

SCENARIO_METHOD(ChildRelicBatchTestsFixture, "child relic batch")
{
    GIVEN("parented dynamic relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Relic<BasicTypedRelic>()
            .Actualize();

        const auto parent = reliquary.CreateRelic<BasicTypedRelic>();
        const auto child = reliquary.CreateRelic<BasicTypedRelic>();
        reliquary.ParentRelic(parent->ID(), child->ID());

        WHEN("retrieving child relic batch")
        {
            auto batch = reliquary.ChildRelicBatch<BasicTypedRelic>(parent->ID());

            THEN("batch is occupied")
            {
                REQUIRE(!batch.IsEmpty());
            }
        }
    }
}