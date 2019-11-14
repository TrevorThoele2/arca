#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "RelicTests.h"

#include <Arca/ExtractShards.h>

#include <Chroma/StringUtility.h>

RelicTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

RelicTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

RelicTestsFixture::BasicTypedRelic::BasicTypedRelic(const ::Inscription::BinaryTableData<BasicTypedRelic>& data) :
    TypedRelic(data.base)
{}

void RelicTestsFixture::BasicTypedRelic::Initialize(Reliquary& reliquary)
{
    auto tuple = ExtractShards<Shards>(ID(), reliquary);
    basicShard = std::get<0>(tuple);
}

RelicStructure RelicTestsFixture::BasicTypedRelic::Structure() const
{
    return StructureFrom<Shards>();
}

RelicTestsFixture::StaticRelic::StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data) :
    TypedRelic(data.base)
{}

void RelicTestsFixture::StaticRelic::Initialize(Reliquary& reliquary)
{
    auto tuple = ExtractShards<Shards>(ID(), reliquary);
    basicShard = std::get<0>(tuple);
}

RelicStructure RelicTestsFixture::StaticRelic::Structure() const
{
    return StructureFrom<Shards>();
}

namespace Arca
{
    const TypeHandle ShardTraits<RelicTestsFixture::BasicShard>::typeHandle =
        "RelicTestsBasicShard";

    const TypeHandle ShardTraits<RelicTestsFixture::OtherShard>::typeHandle =
        "ReliquaryTestsOtherShard";

    const TypeHandle RelicTraits<RelicTestsFixture::BasicTypedRelic>::typeHandle =
        "ReliquaryTestsBasicTypedRelic";

    const TypeHandle RelicTraits<RelicTestsFixture::StaticRelic>::typeHandle =
        "ReliquaryTestsStaticRelic";
}

SCENARIO_METHOD(RelicTestsFixture, "relic", "[relic]")
{
    GIVEN("all types registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Shard<OtherShard>()
            .StaticRelic<StaticRelic>()
            .Actualize();

        WHEN("creating dynamic relic")
        {
            auto preCreateRelicCount = reliquary.RelicCount();
            auto relic = reliquary.CreateRelic();

            THEN("relic is dynamic")
            {
                REQUIRE(relic.Dynamism() == RelicDynamism::Dynamic);
            }

            THEN("reliquary relic count increments by one")
            {
                REQUIRE(reliquary.RelicCount() == (preCreateRelicCount + 1));
            }

            WHEN("creating shard")
            {
                auto shard = relic.CreateShard<BasicShard>();

                THEN("shard is returned")
                {
                    REQUIRE(shard != nullptr);
                }

                THEN("relic has shard")
                {
                    REQUIRE(relic.FindShard<BasicShard>() != nullptr);
                    REQUIRE(relic.HasShard<BasicShard>());
                }

                WHEN("destroying shard")
                {
                    relic.DestroyShard<BasicShard>();

                    THEN("relic does not have shard")
                    {
                        REQUIRE(relic.FindShard<BasicShard>() == nullptr);
                        REQUIRE(!relic.HasShard<BasicShard>());
                    }
                }
            }
        }

        WHEN("creating fixed relic with valid structure")
        {
            auto preCreateRelicCount = reliquary.RelicCount();
            auto relic = reliquary.CreateRelic(RelicStructure{ ShardTraits<BasicShard>::typeHandle });

            THEN("structure has been satisfied")
            {
                auto shardFromReliquary = reliquary.FindShard<BasicShard>(relic.ID());
                REQUIRE(shardFromReliquary != nullptr);

                auto shardFromRelic = relic.FindShard<BasicShard>();
                REQUIRE(shardFromRelic != nullptr);
                REQUIRE(relic.HasShard<BasicShard>());
            }

            THEN("shards cannot be created later")
            {
                REQUIRE_THROWS_AS(relic.CreateShard<OtherShard>(), CannotCreateShard);
            }

            THEN("shards cannot be destroyed later")
            {
                REQUIRE_THROWS_AS(relic.DestroyShard<OtherShard>(), CannotDestroyShard);
            }

            THEN("relic is fixed")
            {
                REQUIRE(relic.Dynamism() == RelicDynamism::Fixed);
            }

            THEN("reliquary relic count increments by one")
            {
                REQUIRE(reliquary.RelicCount() == (preCreateRelicCount + 1));
            }

            WHEN("destroying relic")
            {
                auto preDestroyRelicCount = reliquary.RelicCount();

                auto id = relic.ID();
                reliquary.DestroyRelic(relic);

                THEN("finding relic returns empty")
                {
                    auto found = reliquary.FindRelic(id);
                    REQUIRE(!found.has_value());
                }

                THEN("destroying again does not throw")
                {
                    REQUIRE_NOTHROW(reliquary.DestroyRelic(relic));
                }

                THEN("reliquary relic count decrements by one")
                {
                    REQUIRE(reliquary.RelicCount() == (preDestroyRelicCount - 1));
                }
            }
        }

        WHEN("retrieving static relic")
        {
            const auto staticRelic = reliquary.StaticRelic<StaticRelic>();

            THEN("structure has been satisfied")
            {
                auto shardFromReliquary = reliquary.FindShard<BasicShard>(staticRelic->ID());
                REQUIRE(shardFromReliquary != nullptr);

                auto shardFromRelic = staticRelic->basicShard;
                REQUIRE(shardFromRelic != nullptr);
            }

            THEN("cannot destroy relic")
            {
                auto preDestroyRelicCount = reliquary.RelicCount();

                reliquary.DestroyRelic(*staticRelic);

                auto foundAgain = reliquary.StaticRelic<StaticRelic>();

                REQUIRE(foundAgain->ID() == staticRelic->ID());
                REQUIRE(foundAgain->basicShard == staticRelic->basicShard);
                REQUIRE(preDestroyRelicCount == reliquary.RelicCount());
            }
        }

        WHEN("retrieving static relic by id")
        {
            const auto idProvider = reliquary.StaticRelic<StaticRelic>();
            const auto staticRelic = reliquary.FindRelic(idProvider->ID());

            THEN("returns value")
            {
                REQUIRE(staticRelic.has_value());
            }

            THEN("structure has been satisfied")
            {
                auto shardFromReliquary = reliquary.FindShard<BasicShard>(staticRelic->ID());
                REQUIRE(shardFromReliquary != nullptr);

                auto shardFromRelic = staticRelic->FindShard<BasicShard>();
                REQUIRE(shardFromRelic != nullptr);
            }

            THEN("owner is correct")
            {
                REQUIRE(&staticRelic->Owner() == &reliquary);
            }

            THEN("cannot destroy relic")
            {
                auto preDestroyRelicCount = reliquary.RelicCount();

                reliquary.DestroyRelic(*staticRelic);

                auto foundAgain = reliquary.StaticRelic<StaticRelic>();

                REQUIRE(foundAgain->ID() == staticRelic->ID());
                REQUIRE(foundAgain->basicShard == staticRelic->FindShard<BasicShard>());
                REQUIRE(preDestroyRelicCount == reliquary.RelicCount());
            }
        }

        WHEN("retrieving unregistered static relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.StaticRelic<BasicTypedRelic>(),
                    NotRegistered,
                    ::Catch::Matchers::Message("The static relic (ReliquaryTestsBasicTypedRelic) was not registered.")
                );
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "many relics", "[relic]")
{
    GIVEN("many dynamic relics")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Shard<OtherShard>()
            .StaticRelic<StaticRelic>()
            .Actualize();

        std::vector<DynamicRelic> relics;
        for (size_t i = 0; i < 100; ++i)
        {
            relics.push_back(reliquary.CreateRelic());
            relics.back().CreateShard<BasicShard>()->myValue = ::Chroma::ToString(i);
        }

        WHEN("deleting all but the last")
        {
            auto loop = relics.begin();
            while (loop != --relics.end())
            {
                reliquary.DestroyRelic(*loop);
                loop = relics.erase(loop);
            }

            THEN("last relic still valid")
            {
                auto relic = relics[0];

                REQUIRE(relic.HasShard<BasicShard>() == true);

                auto shard = relic.FindShard<BasicShard>();
                REQUIRE(shard != nullptr);
                REQUIRE(shard->myValue == ::Chroma::ToString(99));
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "relic signals", "[relic][signal]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<BasicTypedRelic>()
            .Shard<BasicShard>()
            .Actualize();

        auto createdSignals = reliquary.SignalBatch<RelicCreated<BasicTypedRelic>>();
        auto destroyedSignals = reliquary.SignalBatch<BeforeRelicDestroyed<BasicTypedRelic>>();

        WHEN("creating relic")
        {
            auto created = reliquary.CreateRelic<BasicTypedRelic>();

            THEN("signal is emitted")
            {
                REQUIRE(createdSignals.Size() == 1);
            }

            WHEN("destroying relic")
            {
                reliquary.DestroyRelic(*created);

                THEN("signal is emitted")
                {
                    REQUIRE(destroyedSignals.Size() == 1);
                }
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "relic parenting", "[relic][parenting]")
{
    GIVEN("static relic and dynamic relic")
    {
        auto reliquary = ReliquaryOrigin()
            .StaticRelic<StaticRelic>()
            .Shard<BasicShard>()
            .Actualize();

        auto staticRelic = reliquary.StaticRelic<StaticRelic>();
        auto dynamicRelic = reliquary.CreateRelic();

        WHEN("parenting child to static parent")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.ParentRelic(staticRelic->ID(), dynamicRelic.ID()),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + Chroma::ToString(dynamicRelic.ID()) + ") " +
                        "was attempted to be parented to a static relic.")
                );
            }
        }

        WHEN("parenting static child to parent")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.ParentRelic(dynamicRelic.ID(), staticRelic->ID()),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + Chroma::ToString(staticRelic->ID()) + ") " +
                        "is static and cannot be parented to anything.")
                );
            }
        }
    }

    GIVEN("parent and child created")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Actualize();

        auto parent = reliquary.CreateRelic();
        parent.CreateShard<BasicShard>();
        auto child = reliquary.CreateRelic();
        child.CreateShard<BasicShard>();

        WHEN("parenting child to parent")
        {
            reliquary.ParentRelic(parent.ID(), child.ID());

            THEN("destroying parent also destroys child")
            {
                reliquary.DestroyRelic(parent);

                REQUIRE(reliquary.RelicCount() == 0);
                REQUIRE(reliquary.FindShard<BasicShard>(child.ID()) == nullptr);
            }

            WHEN("destroying child")
            {
                reliquary.DestroyRelic(child);

                THEN("does not destroy parent")
                {
                    REQUIRE(reliquary.RelicCount() == 1);
                    REQUIRE(reliquary.FindShard<BasicShard>(parent.ID()) != nullptr);
                }

                THEN("destroying parent works")
                {
                    reliquary.DestroyRelic(parent);

                    REQUIRE(reliquary.RelicCount() == 0);
                    REQUIRE(reliquary.FindShard<BasicShard>(parent.ID()) == nullptr);
                }
            }

            WHEN("parenting child to parent again")
            {
                THEN("throws error")
                {
                    REQUIRE_THROWS_MATCHES
                    (
                        reliquary.ParentRelic(parent.ID(), child.ID()),
                        CannotParentRelic,
                        ::Catch::Matchers::Message(
                            "The relic with id("s + Chroma::ToString(child.ID()) + ") is already parented.")
                    );
                }
            }
        }

        WHEN("parenting child to nonexistent relic")
        {
            THEN("throws error")
            {
                auto invalidId = -1;

                REQUIRE_THROWS_MATCHES
                (
                    reliquary.ParentRelic(invalidId, child.ID()),
                    CannotFindRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + ::Chroma::ToString(invalidId) + ") cannot be found.")
                );
            }
        }

        WHEN("parenting nonexistent relic to parent")
        {
            THEN("throws error")
            {
                auto invalidId = -1;

                REQUIRE_THROWS_MATCHES
                (
                    reliquary.ParentRelic(parent.ID(), invalidId),
                    CannotFindRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + ::Chroma::ToString(invalidId) + ") cannot be found.")
                );
            }
        }

        WHEN("parenting relic to itself")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary.ParentRelic(parent.ID(), parent.ID()),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + Chroma::ToString(parent.ID()) + ") " +
                        "was attempted to be parented to itself.")
                );
            }
        }
    }
}