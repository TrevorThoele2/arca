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

RelicBatchFixture::StaticVessel::StaticVessel(VesselID id, Reliquary& owner) : TypedVessel(id, owner)
{
    Setup();
}

void RelicBatchFixture::StaticVessel::Setup()
{
    ExtractRelics(RelicTuple(relic));
}

namespace Arca
{
    const TypeHandle RelicTraits<RelicBatchFixture::Relic>::typeHandle = "RelicBatchTestsRelic";
    const TypeHandle RelicTraits<RelicBatchFixture::UnregisteredRelic>::typeHandle = "RelicBatchTestsUnregisteredRelic";
    const TypeHandle VesselTraits<RelicBatchFixture::StaticVessel>::typeHandle = "RelicBatchTestsStaticVessel";
}

SCENARIO_METHOD(RelicBatchFixture, "default relic batch", "[RelicBatch]")
{
    GIVEN("default relic batch")
    {
        RelicBatch<Relic> batch;

        WHEN("querying size")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.Size(), RelicBatchNotSetup);
            }
        }

        WHEN("querying empty")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.IsEmpty(), RelicBatchNotSetup);
            }
        }

        WHEN("querying begin")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.begin(), RelicBatchNotSetup);
            }
        }

        WHEN("querying begin const")
        {
            auto& constBatch = batch;

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(constBatch.begin(), RelicBatchNotSetup);
            }
        }

        WHEN("querying end")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.end(), RelicBatchNotSetup);
            }
        }

        WHEN("querying begin const")
        {
            auto& constBatch = batch;

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(constBatch.end(), RelicBatchNotSetup);
            }
        }
    }
}

SCENARIO_METHOD(RelicBatchFixture, "relic batch", "[RelicBatch]")
{
    GIVEN("registered reliquary and vessel")
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

                THEN("batch contains relic")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned relic is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == createdRelic);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
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

            THEN("batch is empty")
            {
                REQUIRE(batch.IsEmpty());
                REQUIRE(batch.Size() == 0);
            }

            THEN("begin is end")
            {
                REQUIRE(batch.begin() == batch.end());
            }

            WHEN("creating derived relic")
            {
                auto createdRelic = vessel.CreateRelic<Relic>();
                
                THEN("batch contains derived relic")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("returned relic is referentially equal to beginning")
                {
                    auto& first = *batch.begin();
                    REQUIRE(&first == createdRelic);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("removing relic empties the batch")
                {
                    vessel.DestroyRelic<Relic>();
                    REQUIRE(batch.IsEmpty());
                }
            }
        }
    }

    GIVEN("registered reliquary with static vessel")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<Relic>()
            .StaticVessel<StaticVessel>()
            .Actualize();

        WHEN("starting batch with unregistered relic")
        {
            THEN("throws")
            {
                REQUIRE_THROWS_AS(reliquary.StartRelicBatch<UnregisteredRelic>(), NotRegistered);
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary.StartRelicBatch<Relic>();

            THEN("batch contains relic")
            {
                REQUIRE(!batch.IsEmpty());
                REQUIRE(batch.Size() == 1);
            }

            THEN("begin is not end")
            {
                REQUIRE(batch.begin() != batch.end());
            }
        }
    }
}

SCENARIO_METHOD(RelicBatchFixture, "relic batch serialization", "[RelicBatch][serialization]")
{
    GIVEN("saved reliquary ")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Relic<Relic>()
            .Actualize();

        auto savedVessel = savedReliquary.CreateVessel();
        savedVessel.CreateRelic<Relic>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.exe", "Testing", 1);
            outputArchive(savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Relic<Relic>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.exe", "Testing");
                inputArchive(loadedReliquary);
            }

            auto batch = loadedReliquary.StartRelicBatch<Relic>();

            THEN("batch is occupied")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
            }
        }
    }
}