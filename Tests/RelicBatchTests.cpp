#include <catch.hpp>

#include "RelicBatchTests.h"

#include "BasicShard.h"

#include <Arca/Create.h>
#include <Arca/Destroy.h>

#include <Inscription/Binary.h>

SCENARIO_METHOD(RelicBatchFixture, "default relic batch", "[RelicBatch]")
{
    GIVEN("default relic batch")
    {
        Batch<Relic> batch;

        WHEN("querying size")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.Size(), BatchNotSetup);
            }
        }

        WHEN("querying empty")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.IsEmpty(), BatchNotSetup);
            }
        }

        WHEN("querying begin")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.begin(), BatchNotSetup);
            }
        }

        WHEN("querying begin const")
        {
            const auto& constBatch = batch;

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(constBatch.begin(), BatchNotSetup);
            }
        }

        WHEN("querying end")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(batch.end(), BatchNotSetup);
            }
        }

        WHEN("querying begin const")
        {
            const auto& constBatch = batch;

            THEN("throws error")
            {
                REQUIRE_THROWS_AS(constBatch.end(), BatchNotSetup);
            }
        }

        WHEN("querying owner")
        {
            THEN("returns nullptr")
            {
                REQUIRE_THROWS_AS(batch.Owner(), BatchNotSetup);
            }
        }
    }
}

SCENARIO_METHOD(RelicBatchFixture, "relic batch", "[RelicBatch]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Relic>()
            .Actualize();

        WHEN("creating relic")
        {
            auto createdRelic = reliquary->Do(Create<Relic>());

            WHEN("starting batch")
            {
                auto batch = reliquary->Batch<Relic>();

                THEN("batch contains relic")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("destroying relic empties the batch")
                {
                    reliquary->Destroy(AsHandle(createdRelic));
                    REQUIRE(batch.IsEmpty());
                }

                THEN("owner is reliquary")
                {
                    REQUIRE(&batch.Owner() == reliquary.get());
                }
            }
        }

        WHEN("starting batch")
        {
            auto batch = reliquary->Batch<Relic>();

            THEN("batch is empty")
            {
                REQUIRE(batch.IsEmpty());
                REQUIRE(batch.Size() == 0);
            }

            THEN("begin is end")
            {
                REQUIRE(batch.begin() == batch.end());
            }

            THEN("owner is reliquary")
            {
                REQUIRE(&batch.Owner() == reliquary.get());
            }

            WHEN("creating relic")
            {
                auto createdRelic = reliquary->Do(Create<Relic>());

                THEN("batch contains relic")
                {
                    REQUIRE(!batch.IsEmpty());
                    REQUIRE(batch.Size() == 1);
                }

                THEN("begin is not end")
                {
                    REQUIRE(batch.begin() != batch.end());
                }

                THEN("destroying relic empties the batch")
                {
                    reliquary->Destroy(AsHandle(createdRelic));
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("creating multiple relics")
        {
            for (size_t i = 0; i < 10; ++i)
                reliquary->Do(Arca::Create<Relic>());

            WHEN("iterating over batch and destroying current relic")
            {
                std::vector<RelicID> iteratedIDs;

                auto batch = reliquary->Batch<Relic>();

                for(auto relic = batch.begin(); relic != batch.end();)
                {
                    iteratedIDs.push_back(relic.ID());
                    auto next = relic;
                    ++next;
                    reliquary->Do(Arca::Destroy<Relic>(relic.ID()));
                    relic = next;
                }

                THEN("iterates over entire batch")
                {
                    REQUIRE(iteratedIDs.size() == 10);
                }
            }
        }
    }
}

SCENARIO_METHOD(RelicBatchFixture, "relic batch serialization", "[RelicBatch][serialization]")
{
    GIVEN("saved reliquary")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<Relic>()
            .Actualize();

        savedReliquary->Do(Create<Relic>());

        Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<Relic>()
                .Actualize();

            Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

            auto batch = loadedReliquary->Batch<Relic>();

            THEN("batch is occupied")
            {
                REQUIRE(batch.Size() == 1);
                REQUIRE(!batch.IsEmpty());
            }
        }
    }
}