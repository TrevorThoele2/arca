#include <catch.hpp>

#include "RelicBatchTests.h"

RelicBatchFixture::RelicBatchFixture()
{
    auto typeGroup = typeRegistration.CreateGroup();
    typeGroup->RegisterRelic<AbstractRelic>();
    typeGroup->RegisterRelic<DerivedRelic>();
}

RelicBatchFixture::AbstractRelic::~AbstractRelic() = default;

RelicBatchFixture::AbstractRelic::AbstractRelic(const std::string& abstractValue) : abstractValue(abstractValue)
{}

RelicBatchFixture::AbstractRelic::AbstractRelic(const ::Inscription::BinaryTableData<AbstractRelic>& data) :
    abstractValue(data.abstractValue)
{}

RelicBatchFixture::DerivedRelic::DerivedRelic(const std::string& abstractValue, int derivedValue) :
    AbstractRelic(abstractValue), derivedValue(derivedValue)
{}

RelicBatchFixture::DerivedRelic::DerivedRelic(const ::Inscription::BinaryTableData<DerivedRelic>& data) :
    AbstractRelic(std::get<0>(data.bases)), derivedValue(data.derivedValue)
{}

namespace Arca
{
    const TypeHandle RelicTraits<RelicBatchFixture::AbstractRelic>::typeHandle = "RelicBatchTestsAbstractRelic";
    const TypeHandle RelicTraits<RelicBatchFixture::DerivedRelic>::typeHandle = "RelicBatchTestsDerivedRelic";
    const TypeHandle RelicTraits<RelicBatchFixture::UnregisteredRelic>::typeHandle = "RelicBatchTestsUnregisteredRelic";
}

namespace Inscription
{
    Scribe<::RelicBatchFixture::AbstractRelic, BinaryArchive>::Table::Table()
    {
        MergeDataLinks
        ({
            DataLink::Auto(&ObjectT::abstractValue, &DataT::abstractValue) }
        );
    }

    Scribe<::RelicBatchFixture::DerivedRelic, BinaryArchive>::Table::Table()
    {
        MergeDataLinks
        ({
            DataLink::Auto(&ObjectT::derivedValue, &DataT::derivedValue) }
        );
    }
}

SCENARIO_METHOD(RelicBatchFixture, "RelicBatch")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = CreateRegistered<Reliquary>();

        WHEN("starting batch with unregistered relic")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_AS(reliquary.StartRelicBatch<UnregisteredRelic>(), NotRegistered);
            }
        }

        WHEN("initialized reliquary")
        {
            reliquary.Initialize();

            WHEN("creating derived relic")
            {
                auto& createdRelic = reliquary.CreateRelic<DerivedRelic>(
                    dataGeneration.Random<std::string>(),
                    dataGeneration.Random<int>());
                auto createdID = reliquary.IDFor(createdRelic);

                WHEN("starting derived batch")
                {
                    auto batch = reliquary.StartRelicBatch<DerivedRelic>();
                    auto& first = **batch.begin();

                    THEN("batch contains derived relic")
                    {
                        REQUIRE(!batch.IsEmpty());
                        REQUIRE(batch.Size() == 1);
                        REQUIRE(first.abstractValue == createdRelic.abstractValue);
                        REQUIRE(first.derivedValue == createdRelic.derivedValue);
                        REQUIRE(&first == &createdRelic);
                    }

                    THEN("removing relic empties the batch")
                    {
                        batch.Remove(batch.begin());
                        REQUIRE(batch.IsEmpty());
                    }
                }

                WHEN("starting base batch")
                {
                    auto batch = reliquary.StartRelicBatch<AbstractRelic>();
                    auto& first = **batch.begin();

                    THEN("base batch is occupied")
                    {
                        REQUIRE(!batch.IsEmpty());
                        REQUIRE(batch.Size() == 1);
                        REQUIRE(first.abstractValue == createdRelic.abstractValue);
                        REQUIRE(&first == &createdRelic);
                    }
                }
            }

            WHEN("starting derived batch")
            {
                auto batch = reliquary.StartRelicBatch<DerivedRelic>();

                REQUIRE(batch.IsEmpty());
                REQUIRE(batch.Size() == 0);

                WHEN("creating derived relic")
                {
                    auto& createdRelic = reliquary.CreateRelic<DerivedRelic>(
                        dataGeneration.Random<std::string>(),
                        dataGeneration.Random<int>());
                    auto& first = **batch.begin();

                    THEN("batch contains derived relic")
                    {
                        REQUIRE(!batch.IsEmpty());
                        REQUIRE(batch.Size() == 1);
                        REQUIRE(first.abstractValue == createdRelic.abstractValue);
                        REQUIRE(first.derivedValue == createdRelic.derivedValue);
                        REQUIRE(&first == &createdRelic);
                    }

                    THEN("removing relic empties the batch")
                    {
                        batch.Remove(batch.begin());
                        REQUIRE(batch.IsEmpty());
                    }
                }
            }
        }
    }
}