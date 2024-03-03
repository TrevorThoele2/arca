#include <catch.hpp>

#include "ConstShardTests.h"

#include <Arca/OpenRelic.h>
#include <Arca/Create.h>

ConstShardTestsFixture::Shard::Shard(int value) :
    value(value)
{}

ConstShardTestsFixture::Relic::Relic(RelicInit init)
{
    shard = init.Create<const Shard>();
}

SCENARIO_METHOD(ConstShardTestsFixture, "const shards", "[shard][const]")
{
    GIVEN("open relic created")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<Shard>()
            .Actualize();

        auto relic = reliquary->Do(Create<OpenRelic>());

        WHEN("creating const shard on relic")
        {
            auto createdShard = reliquary->Do(Create<const Shard>(relic.ID()));

            THEN("is occupied")
            {
                REQUIRE(createdShard);
                REQUIRE(createdShard->value == 100);
            }

            WHEN("finding const shard")
            {
                auto foundShard = reliquary->Find<const Shard>(relic.ID());

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
                auto foundShard = reliquary->Find<Shard>(relic.ID());

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

            reliquary->Do(Create<Shard>(createdRelic1));
            reliquary->Do(Create<Shard>(createdRelic2));
            reliquary->Do(Create<Shard>(createdRelic3));

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

            reliquary->Do(Create<const Shard>(createdRelic1));
            reliquary->Do(Create<const Shard>(createdRelic2));
            reliquary->Do(Create<const Shard>(createdRelic3));

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

            reliquary->Do(Create<Shard>(createdRelic1));
            reliquary->Do(Create<const Shard>(createdRelic2));
            reliquary->Do(Create<Shard>(createdRelic3));
            reliquary->Do(Create<const Shard>(createdRelic4));
            reliquary->Do(Create<Shard>(createdRelic5));
            reliquary->Do(Create<const Shard>(createdRelic6));

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
            .Register<OpenRelic>()
            .Register<Shard>()
            .Actualize();

        auto relicStructure = RelicStructure{};
        relicStructure.emplace_back(TypeFor<const Shard>());
        auto relic = reliquary->Do(Create<OpenRelic>{ CreateData{ .structure = relicStructure } });

        WHEN("finding const shard")
        {
            auto foundShard = reliquary->Find<const Shard>(relic.ID());

            THEN("is occupied")
            {
                REQUIRE(foundShard);
                REQUIRE(foundShard->value == 100);
            }
        }

        WHEN("finding non-const shard")
        {
            auto foundShard = reliquary->Find<Shard>(relic.ID());

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
            .Register<OpenRelic>()
            .Register<Shard>()
            .Actualize();

        auto savedRelic = savedReliquary->Do(Create<OpenRelic>());
        savedReliquary->Do(Create<const Shard>(savedRelic));

        {
            auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
            outputArchive(*savedReliquary);
        }

        WHEN("loading reliquary")
        {
            auto loadedReliquary = ReliquaryOrigin()
                .Register<OpenRelic>()
                .Register<Shard>()
                .Actualize();

            {
                auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                inputArchive(*loadedReliquary);
            }

            auto loadedRelic = loadedReliquary->Find<OpenRelic>(savedRelic.ID());

            WHEN("finding const shard")
            {
                auto foundShard = loadedReliquary->Find<const Shard>(loadedRelic.ID());

                THEN("is occupied")
                {
                    REQUIRE(foundShard);
                    REQUIRE(foundShard->value == 100);
                }
            }

            WHEN("finding non-const shard")
            {
                auto foundShard = loadedReliquary->Find<Shard>(loadedRelic.ID());

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
                    REQUIRE(&*batch.begin() == static_cast<const Shard*>(loadedReliquary->Find<const Shard>(loadedRelic.ID())));
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