#include <catch.hpp>

#include "RelicBatchTests.h"

RelicBatchFixture::RelicBatchFixture()
{
    auto typeGroup = typeRegistration.CreateGroup();
    typeGroup->RegisterRelic<Relic>();
}

RelicBatchFixture::Relic::Relic(int value) :
    value(value)
{}

RelicBatchFixture::Relic::Relic(const ::Inscription::BinaryTableData<Relic>& data) :
    value(data.value)
{}

namespace Arca
{
    const TypeHandle RelicTraits<RelicBatchFixture::Relic>::typeHandle = "RelicBatchTestsRelic";
    const TypeHandle RelicTraits<RelicBatchFixture::UnregisteredRelic>::typeHandle = "RelicBatchTestsUnregisteredRelic";
}

namespace Inscription
{
    Scribe<::RelicBatchFixture::Relic, BinaryArchive>::Table::Table()
    {
        MergeDataLinks
        ({
            DataLink::Auto(&ObjectT::value, &DataT::value) }
        );
    }
}

SCENARIO_METHOD(RelicBatchFixture, "RelicBatch")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = CreateRegistered<Reliquary>();
        auto vessel = reliquary.CreateVessel();

        WHEN("starting batch with unregistered relic")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_AS(reliquary.StartRelicBatch<UnregisteredRelic>(), NotRegistered);
            }
        }

        WHEN("creating relic")
        {
            auto createdRelic = vessel.CreateRelic<Relic>();

            WHEN("starting batch")
            {
                auto batch = reliquary.StartRelicBatch<Relic>();
                auto& first = *batch.begin();

                THEN("batch contains relic")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                    REQUIRE(first.value == createdRelic->value);
                    REQUIRE(&first == createdRelic);
                }

                THEN("removing relic empties the batch")
                {
                    vessel.DestroyRelic<Relic>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary.StartRelicBatch<Relic>();

            REQUIRE(batch.IsEmpty());
            REQUIRE(batch.Size() == 0);

            WHEN("creating derived relic")
            {
                auto createdRelic = vessel.CreateRelic<Relic>();
                auto& first = *batch.begin();

                THEN("batch contains derived relic")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                    REQUIRE(first.value == createdRelic->value);
                    REQUIRE(&first == createdRelic);
                }

                THEN("removing relic empties the batch")
                {
                    vessel.DestroyRelic<Relic>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }
    }
}