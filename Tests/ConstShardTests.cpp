#include <catch.hpp>

#include "ConstShardTests.h"

#include <Arca/ExtractShards.h>

ConstShardTestsFixture::Shard::Shard(int value) :
    value(value)
{}

void ConstShardTestsFixture::Relic::InitializeImplementation()
{
    auto shards = Arca::ExtractShards<shards_for_t<Relic>>(ID(), Owner());
    shard = std::get<0>(shards);
}

namespace Arca
{
    const TypeHandleName Traits<ConstShardTestsFixture::Shard>::typeName =
        "ConstShardTestsShard";

    const TypeHandleName Traits<ConstShardTestsFixture::Relic>::typeName =
        "ConstShardTestsRelic";
}

SCENARIO_METHOD(ConstShardTestsFixture, "const shards", "[shard][const]")
{
    GIVEN("dynamic relic created")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .Actualize();

        auto relic = reliquary->Create<DynamicRelic>();

        WHEN("creating const shard on relic")
        {
            auto createdShard = relic.Create<const Shard>();

            THEN("is occupied")
            {
                REQUIRE(createdShard);
                REQUIRE(createdShard->value == 100);
            }

            WHEN("finding const shard")
            {
                auto foundShard = relic.Find<const Shard>();

                THEN("is occupied")
                {
                    REQUIRE(foundShard);
                    REQUIRE(foundShard->value == 100);
                }

                THEN("found is same as created")
                {
                    REQUIRE(foundShard == createdShard);
                }
            }

            WHEN("finding non-const shard")
            {
                auto foundShard = relic.Find<Shard>();

                THEN("is not occupied")
                {
                    REQUIRE(!foundShard);
                }
            }

            WHEN("retrieving const batch")
            {
                auto batch = reliquary->Batch<const Shard>();

                THEN("is occupied")
                {
                    REQUIRE(!batch.IsEmpty());
                }

                THEN("begin is same as created")
                {
                    REQUIRE(&*batch.begin() == static_cast<const Shard*>(createdShard));
                }
            }

            WHEN("retrieving non-const batch")
            {
                auto batch = reliquary->Batch<Shard>();

                THEN("is not occupied")
                {
                    REQUIRE(batch.IsEmpty());
                }

                THEN("begin is same as end")
                {
                    REQUIRE(batch.begin() == batch.end());
                }
            }
        }

        WHEN("creating many non-const shards on relics")
        {
            auto createdRelic1 = reliquary->Create<DynamicRelic>();
            auto createdRelic2 = reliquary->Create<DynamicRelic>();
            auto createdRelic3 = reliquary->Create<DynamicRelic>();

            createdRelic1.Create<Shard>();
            createdRelic2.Create<Shard>();
            createdRelic3.Create<Shard>();

            WHEN("retrieving const shard batch")
            {
                auto batch = reliquary->Batch<const Shard>();

                THEN("is empty")
                {
                    REQUIRE(batch.IsEmpty());
                }
            }

            WHEN("retrieving shard batch")
            {
                auto batch = reliquary->Batch<Shard>();

                THEN("has size 3")
                {
                    REQUIRE(batch.Size() == 3);
                }
            }
        }

        WHEN("creating many const shards on relics")
        {
            auto createdRelic1 = reliquary->Create<DynamicRelic>();
            auto createdRelic2 = reliquary->Create<DynamicRelic>();
            auto createdRelic3 = reliquary->Create<DynamicRelic>();

            createdRelic1.Create<const Shard>();
            createdRelic2.Create<const Shard>();
            createdRelic3.Create<const Shard>();

            WHEN("retrieving const shard batch")
            {
                auto batch = reliquary->Batch<const Shard>();

                THEN("has size 3")
                {
                    REQUIRE(batch.Size() == 3);
                }
            }

            WHEN("retrieving shard batch")
            {
                auto batch = reliquary->Batch<Shard>();

                THEN("is empty")
                {
                    REQUIRE(batch.Size() == 0);
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("creating many non-const and const shards on relics")
        {
            auto createdRelic1 = reliquary->Create<DynamicRelic>();
            auto createdRelic2 = reliquary->Create<DynamicRelic>();
            auto createdRelic3 = reliquary->Create<DynamicRelic>();
            auto createdRelic4 = reliquary->Create<DynamicRelic>();
            auto createdRelic5 = reliquary->Create<DynamicRelic>();
            auto createdRelic6 = reliquary->Create<DynamicRelic>();

            createdRelic1.Create<Shard>();
            createdRelic2.Create<const Shard>();
            createdRelic3.Create<Shard>();
            createdRelic4.Create<const Shard>();
            createdRelic5.Create<Shard>();
            createdRelic6.Create<const Shard>();

            WHEN("retrieving const shard batch")
            {
                auto batch = reliquary->Batch<const Shard>();

                THEN("has size 3")
                {
                    REQUIRE(batch.Size() == 3);
                }
            }

            WHEN("retrieving shard batch")
            {
                auto batch = reliquary->Batch<Shard>();

                THEN("has size 3")
                {
                    REQUIRE(batch.Size() == 3);
                }
            }
        }
    }

    GIVEN("relic created from relic structure")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .Actualize();

        auto relicStructure = RelicStructure{};
        relicStructure.emplace_back(TypeHandleFor<const Shard>());
        auto relic = reliquary->Create<FixedRelic>(relicStructure);

        WHEN("finding const shard")
        {
            auto foundShard = relic.Find<const Shard>();

            THEN("is occupied")
            {
                REQUIRE(foundShard);
                REQUIRE(foundShard->value == 100);
            }
        }

        WHEN("finding non-const shard")
        {
            auto foundShard = relic.Find<Shard>();

            THEN("is not occupied")
            {
                REQUIRE(!foundShard);
            }
        }

        WHEN("retrieving const batch")
        {
            auto batch = reliquary->Batch<const Shard>();

            THEN("is occupied")
            {
                REQUIRE(!batch.IsEmpty());
            }
        }

        WHEN("retrieving non-const batch")
        {
            auto batch = reliquary->Batch<Shard>();

            THEN("is not occupied")
            {
                REQUIRE(batch.IsEmpty());
            }

            THEN("begin is same as end")
            {
                REQUIRE(batch.begin() == batch.end());
            }
        }
    }

    GIVEN("relic created from typed relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .Relic<Relic>()
            .Actualize();

        auto relic = reliquary->Create<Relic>();

        WHEN("finding const shard")
        {
            THEN("is occupied")
            {
                REQUIRE(relic->shard);
                REQUIRE(relic->shard->value == 100);
            }
        }

        WHEN("retrieving const batch")
        {
            auto batch = reliquary->Batch<const Shard>();

            THEN("is const")
            {
                REQUIRE(std::is_same_v<decltype(batch), Batch<const Shard>>);
            }

            THEN("is occupied")
            {
                REQUIRE(!batch.IsEmpty());
            }
        }

        WHEN("retrieving non-const batch")
        {
            auto batch = reliquary->Batch<Shard>();

            THEN("is not occupied")
            {
                REQUIRE(batch.IsEmpty());
            }

            THEN("begin is same as end")
            {
                REQUIRE(batch.begin() == batch.end());
            }
        }
    }
}

SCENARIO_METHOD(ConstShardTestsFixture, "const shard serialization", "[shard][const][serialization]")
{
    GIVEN("saved reliquary with shard")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .Actualize();

        auto savedRelic = savedReliquary->Create<DynamicRelic>();
        savedRelic.Create<const Shard>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Shard<Shard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = loadedReliquary->Find<DynamicRelic>(savedRelic.ID());

            WHEN("finding const shard")
            {
                auto foundShard = loadedRelic->Find<const Shard>();

                THEN("is occupied")
                {
                    REQUIRE(foundShard);
                    REQUIRE(foundShard->value == 100);
                }
            }

            WHEN("finding non-const shard")
            {
                auto foundShard = loadedRelic->Find<Shard>();

                THEN("is not occupied")
                {
                    REQUIRE(!foundShard);
                }
            }

            WHEN("retrieving const batch")
            {
                auto batch = loadedReliquary->Batch<const Shard>();

                THEN("is const")
                {
                    REQUIRE(std::is_same_v<decltype(batch), Batch<const Shard>>);
                }

                THEN("is occupied")
                {
                    REQUIRE(!batch.IsEmpty());
                }

                THEN("begin is same as loaded")
                {
                    REQUIRE(&*batch.begin() == static_cast<const Shard*>(loadedRelic->Find<const Shard>()));
                }
            }

            WHEN("retrieving non-const batch")
            {
                auto batch = loadedReliquary->Batch<Shard>();

                THEN("is not occupied")
                {
                    REQUIRE(batch.IsEmpty());
                }

                THEN("begin is same as end")
                {
                    REQUIRE(batch.begin() == batch.end());
                }
            }
        }
    }
}