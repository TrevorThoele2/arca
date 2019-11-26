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
    const TypeHandle Traits<ConstShardTestsFixture::Shard>::typeHandle =
        "ConstShardTestsShard";

    const TypeHandle Traits<ConstShardTestsFixture::Relic>::typeHandle =
        "ConstShardTestsRelic";
}

SCENARIO_METHOD(ConstShardTestsFixture, "const shards", "[shard][const]")
{
    GIVEN("dynamic relic created")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .Actualize();

        auto relic = reliquary.CreateRelic();

        WHEN("creating const shard on relic")
        {
            auto createdShard = relic.CreateShard<const Shard>();

            THEN("is const")
            {
                REQUIRE(std::is_same_v<decltype(createdShard), const Shard*>);
            }

            THEN("is occupied")
            {
                REQUIRE(createdShard != nullptr);
                REQUIRE(createdShard->value == 100);
            }

            WHEN("finding const shard")
            {
                auto foundShard = relic.FindShard<const Shard>();

                THEN("is const")
                {
                    REQUIRE(std::is_same_v<decltype(foundShard), const Shard*>);
                }

                THEN("is occupied")
                {
                    REQUIRE(foundShard != nullptr);
                    REQUIRE(foundShard->value == 100);
                }

                THEN("found is same as created")
                {
                    REQUIRE(foundShard == createdShard);
                }
            }

            WHEN("finding non-const shard")
            {
                auto foundShard = relic.FindShard<Shard>();

                THEN("is not occupied")
                {
                    REQUIRE(foundShard == nullptr);
                }

                THEN("found is not same as created")
                {
                    REQUIRE(foundShard != createdShard);
                }
            }

            WHEN("retrieving const batch")
            {
                auto batch = reliquary.Batch<const Shard>();

                THEN("is const")
                {
                    REQUIRE(std::is_same_v<decltype(batch), Batch<const Shard>>);
                }

                THEN("is occupied")
                {
                    REQUIRE(!batch.IsEmpty());
                }

                THEN("begin is same as created")
                {
                    REQUIRE(&*batch.begin() == createdShard);
                }
            }

            WHEN("retrieving non-const batch")
            {
                auto batch = reliquary.Batch<Shard>();

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
            auto createdRelic1 = reliquary.CreateRelic();
            auto createdRelic2 = reliquary.CreateRelic();
            auto createdRelic3 = reliquary.CreateRelic();

            createdRelic1.CreateShard<Shard>();
            createdRelic2.CreateShard<Shard>();
            createdRelic3.CreateShard<Shard>();

            WHEN("retrieving const shard batch")
            {
                auto batch = reliquary.Batch<const Shard>();

                THEN("has size 3")
                {
                    REQUIRE(batch.Size() == 3);
                }
            }

            WHEN("retrieving shard batch")
            {
                auto batch = reliquary.Batch<Shard>();

                THEN("has size 3")
                {
                    REQUIRE(batch.Size() == 3);
                }
            }
        }

        WHEN("creating many const shards on relics")
        {
            auto createdRelic1 = reliquary.CreateRelic();
            auto createdRelic2 = reliquary.CreateRelic();
            auto createdRelic3 = reliquary.CreateRelic();

            createdRelic1.CreateShard<const Shard>();
            createdRelic2.CreateShard<const Shard>();
            createdRelic3.CreateShard<const Shard>();

            WHEN("retrieving const shard batch")
            {
                auto batch = reliquary.Batch<const Shard>();

                THEN("has size 3")
                {
                    REQUIRE(batch.Size() == 3);
                }
            }

            WHEN("retrieving shard batch")
            {
                auto batch = reliquary.Batch<Shard>();

                THEN("is empty")
                {
                    REQUIRE(batch.Size() == 0);
                    REQUIRE(batch.IsEmpty());
                }
            }
        }

        WHEN("creating many non-const and const shards on relics")
        {
            auto createdRelic1 = reliquary.CreateRelic();
            auto createdRelic2 = reliquary.CreateRelic();
            auto createdRelic3 = reliquary.CreateRelic();
            auto createdRelic4 = reliquary.CreateRelic();
            auto createdRelic5 = reliquary.CreateRelic();
            auto createdRelic6 = reliquary.CreateRelic();

            createdRelic1.CreateShard<Shard>();
            createdRelic2.CreateShard<const Shard>();
            createdRelic3.CreateShard<Shard>();
            createdRelic4.CreateShard<const Shard>();
            createdRelic5.CreateShard<Shard>();
            createdRelic6.CreateShard<const Shard>();

            WHEN("retrieving const shard batch")
            {
                auto batch = reliquary.Batch<const Shard>();

                THEN("has size 6")
                {
                    REQUIRE(batch.Size() == 6);
                }
            }

            WHEN("retrieving shard batch")
            {
                auto batch = reliquary.Batch<Shard>();

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
        relicStructure.emplace_back(TypeHandleFor<Shard>(), true);
        auto relic = reliquary.CreateRelic(relicStructure);

        WHEN("finding const shard")
        {
            auto foundShard = relic.FindShard<const Shard>();

            THEN("is const")
            {
                REQUIRE(std::is_same_v<decltype(foundShard), const Shard*>);
            }

            THEN("is occupied")
            {
                REQUIRE(foundShard != nullptr);
                REQUIRE(foundShard->value == 100);
            }
        }

        WHEN("finding non-const shard")
        {
            auto foundShard = relic.FindShard<Shard>();

            THEN("is not occupied")
            {
                REQUIRE(foundShard == nullptr);
            }
        }

        WHEN("retrieving const batch")
        {
            auto batch = reliquary.Batch<const Shard>();

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
            auto batch = reliquary.Batch<Shard>();

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

        auto relic = reliquary.Create<Relic>();

        WHEN("finding const shard")
        {
            THEN("is const")
            {
                REQUIRE(std::is_same_v<decltype(relic->shard), const Shard*>);
            }

            THEN("is occupied")
            {
                REQUIRE(relic->shard != nullptr);
                REQUIRE(relic->shard->value == 100);
            }
        }

        WHEN("retrieving const batch")
        {
            auto batch = reliquary.Batch<const Shard>();

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
            auto batch = reliquary.Batch<Shard>();

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

SCENARIO_METHOD(ConstShardTestsFixture, "const shard serializatiohn", "[shard][const][serialization]")
{
    GIVEN("saved reliquary with shard")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Shard<Shard>()
            .Actualize();

        auto savedRelic = savedReliquary.CreateRelic();
        savedRelic.CreateShard<const Shard>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat", "Testing", 1);
            outputArchive(savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Shard<Shard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "Testing");
                inputArchive(loadedReliquary);
            }

            auto loadedRelic = loadedReliquary.FindRelic(savedRelic.ID());

            WHEN("finding const shard")
            {
                auto foundShard = loadedRelic->FindShard<const Shard>();

                THEN("is const")
                {
                    REQUIRE(std::is_same_v<decltype(foundShard), const Shard*>);
                }

                THEN("is occupied")
                {
                    REQUIRE(foundShard != nullptr);
                    REQUIRE(foundShard->value == 100);
                }
            }

            WHEN("finding non-const shard")
            {
                auto foundShard = loadedRelic->FindShard<Shard>();

                THEN("is not occupied")
                {
                    REQUIRE(foundShard == nullptr);
                }
            }

            WHEN("retrieving const batch")
            {
                auto batch = loadedReliquary.Batch<const Shard>();

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
                    REQUIRE(&*batch.begin() == loadedRelic->FindShard<const Shard>());
                }
            }

            WHEN("retrieving non-const batch")
            {
                auto batch = loadedReliquary.Batch<Shard>();

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