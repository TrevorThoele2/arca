#include <catch.hpp>

#include "ConstShardTests.h"

ConstShardTestsFixture::Shard::Shard(int value) :
    value(value)
{}

ConstShardTestsFixture::Relic::Relic(Init init) : ClosedTypedRelic(init)
{
    shard = Create<const Shard>();
}

SCENARIO_METHOD(ConstShardTestsFixture, "const shards", "[shard][const]")
{
    GIVEN("open relic created")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Actualize();

        auto relic = reliquary->Do(Create<OpenRelic>());

        WHEN("creating const shard on relic")
        {
            auto createdShard = relic->Create<const Shard>();

            THEN("is occupied")
            {
                REQUIRE(createdShard);
                REQUIRE(createdShard->value == 100);
            }

            WHEN("finding const shard")
            {
                auto foundShard = relic->Find<const Shard>();

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
                auto foundShard = relic->Find<Shard>();

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
            auto createdRelic1 = reliquary->Do(Create<OpenRelic>());
            auto createdRelic2 = reliquary->Do(Create<OpenRelic>());
            auto createdRelic3 = reliquary->Do(Create<OpenRelic>());

            createdRelic1->Create<Shard>();
            createdRelic2->Create<Shard>();
            createdRelic3->Create<Shard>();

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
            auto createdRelic1 = reliquary->Do(Create<OpenRelic>());
            auto createdRelic2 = reliquary->Do(Create<OpenRelic>());
            auto createdRelic3 = reliquary->Do(Create<OpenRelic>());

            createdRelic1->Create<const Shard>();
            createdRelic2->Create<const Shard>();
            createdRelic3->Create<const Shard>();

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
            auto createdRelic1 = reliquary->Do(Create<OpenRelic>());
            auto createdRelic2 = reliquary->Do(Create<OpenRelic>());
            auto createdRelic3 = reliquary->Do(Create<OpenRelic>());
            auto createdRelic4 = reliquary->Do(Create<OpenRelic>());
            auto createdRelic5 = reliquary->Do(Create<OpenRelic>());
            auto createdRelic6 = reliquary->Do(Create<OpenRelic>());

            createdRelic1->Create<Shard>();
            createdRelic2->Create<const Shard>();
            createdRelic3->Create<Shard>();
            createdRelic4->Create<const Shard>();
            createdRelic5->Create<Shard>();
            createdRelic6->Create<const Shard>();

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
            .Register<Shard>()
            .Actualize();

        auto relicStructure = RelicStructure{};
        relicStructure.emplace_back(TypeFor<const Shard>());
        auto relic = reliquary->Do(CreateWith<ClosedRelic>{ relicStructure });

        WHEN("finding const shard")
        {
            auto foundShard = relic->Find<const Shard>();

            THEN("is occupied")
            {
                REQUIRE(foundShard);
                REQUIRE(foundShard->value == 100);
            }
        }

        WHEN("finding non-const shard")
        {
            auto foundShard = relic->Find<Shard>();

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
            .Register<Shard>()
            .Register<Relic>()
            .Actualize();

        auto relic = reliquary->Do(Create<Relic>());

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
            .Register<Shard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<OpenRelic>());
        savedRelic->Create<const Shard>();

        {
            auto outputArchive = ::Inscription::OutputBinaryArchive("Test.dat");
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<Shard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = Arca::Index<OpenRelic>(savedRelic->ID(), *loadedReliquary);

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