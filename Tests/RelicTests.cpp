#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "RelicTests.h"

#include <Arca/RelicParented.h>
#include <Arca/ExtractShards.h>

#include <Chroma/StringUtility.h>

RelicTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

RelicTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

RelicTestsFixture::BasicTypedRelic::BasicTypedRelic(const ::Inscription::BinaryTableData<BasicTypedRelic>& data) :
    TypedRelic(data.base)
{}

void RelicTestsFixture::BasicTypedRelic::InitializeImplementation()
{
    using Shards = shards_for_t<BasicTypedRelic>;
    auto tuple = ExtractShards<Shards>(ID(), Owner());
    basicShard = std::get<0>(tuple);
}

RelicTestsFixture::StaticRelic::StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data) :
    TypedRelic(data.base)
{}

void RelicTestsFixture::StaticRelic::InitializeImplementation()
{
    using Shards = shards_for_t<StaticRelic>;
    auto tuple = ExtractShards<Shards>(ID(), Owner());
    basicShard = std::get<0>(tuple);
}

namespace Arca
{
    const TypeHandleName Traits<RelicTestsFixture::BasicShard>::typeName =
        "RelicTestsBasicShard";

    const TypeHandleName Traits<RelicTestsFixture::OtherShard>::typeName =
        "ReliquaryTestsOtherShard";

    const TypeHandleName Traits<RelicTestsFixture::BasicTypedRelic>::typeName =
        "ReliquaryTestsBasicTypedRelic";

    const TypeHandleName Traits<RelicTestsFixture::StaticRelic>::typeName =
        "ReliquaryTestsStaticRelic";

    const TypeHandleName Traits<RelicTestsFixture::MostBasicCustomFactoryRelic>::typeName =
        "ReliquaryTestsMostBasicCustomFactoryRelic";

    std::optional<RelicTestsFixture::MostBasicCustomFactoryRelic>
        Traits<RelicTestsFixture::MostBasicCustomFactoryRelic>::Factory(Reliquary& reliquary)
    {
        RelicTestsFixture::MostBasicCustomFactoryRelic relic;
        relic.value = 999;
        return relic;
    }

    const TypeHandleName Traits<RelicTestsFixture::GuardedCustomFactoryRelic>::typeName =
        "ReliquaryTestsGuardedCustomFactoryRelic";

    std::optional<RelicTestsFixture::GuardedCustomFactoryRelic>
        Traits<RelicTestsFixture::GuardedCustomFactoryRelic>::Factory(Reliquary& reliquary, int value)
    {
        if (value < 100)
            return {};

        RelicTestsFixture::GuardedCustomFactoryRelic relic;
        relic.value = value;
        return relic;
    }
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
            auto preCreateRelicSize = reliquary->RelicSize();
            auto relic = reliquary->Create<DynamicRelic>();

            THEN("reliquary has one more relic")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicSize + 1));
            }

            THEN("does not have parent")
            {
                REQUIRE(!relic.Parent());
            }

            WHEN("creating shard")
            {
                auto preCreateShardSize = reliquary->ShardSize();
                auto shard = relic.Create<BasicShard>();

                THEN("reliquary has one more shard")
                {
                    REQUIRE(reliquary->ShardSize() == (preCreateShardSize + 1));
                }

                THEN("shard is returned")
                {
                    REQUIRE(shard);
                }

                THEN("relic has shard")
                {
                    REQUIRE(relic.Find<BasicShard>());
                    REQUIRE(relic.Has<BasicShard>());
                }

                WHEN("destroying shard")
                {
                    relic.Destroy<BasicShard>();

                    THEN("reliquary loses a shard")
                    {
                        REQUIRE(reliquary->ShardSize() == preCreateRelicSize);
                    }

                    THEN("relic does not have shard")
                    {
                        REQUIRE(!relic.Find<BasicShard>());
                        REQUIRE(!relic.Has<BasicShard>());
                    }
                }
            }

            WHEN("retrieving fixed relic with same id")
            {
                auto fixedRelic = reliquary->Find<FixedRelic>(relic.ID());

                THEN("fixed relic is not found")
                {
                    REQUIRE(!fixedRelic);
                }
            }
        }

        WHEN("creating fixed relic with valid structure")
        {
            auto preCreateRelicCount = reliquary->RelicSize();
            auto relic = reliquary->Create<FixedRelic>(RelicStructure{ TypeHandleFor<BasicShard>() });

            THEN("structure has been satisfied")
            {
                REQUIRE(reliquary->Find<BasicShard>(relic.ID()));

                REQUIRE(relic.Find<BasicShard>());
                REQUIRE(relic.Has<BasicShard>());
            }

            THEN("reliquary relic count increments by one")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicCount + 1));
            }

            WHEN("destroying relic")
            {
                auto preDestroyRelicCount = reliquary->RelicSize();

                auto id = relic.ID();
                reliquary->Destroy(relic);

                THEN("finding relic returns empty")
                {
                    REQUIRE(!reliquary->Find<FixedRelic>(id));
                }

                THEN("destroying again does not throw")
                {
                    REQUIRE_NOTHROW(reliquary->Destroy(relic));
                }

                THEN("reliquary relic count decrements by one")
                {
                    REQUIRE(reliquary->RelicSize() == (preDestroyRelicCount - 1));
                }
            }

            WHEN("retrieving dynamic relic with same id")
            {
                auto dynamicRelic = reliquary->Find<DynamicRelic>(relic.ID());

                THEN("dynamic relic is not found")
                {
                    REQUIRE(!dynamicRelic);
                }
            }
        }

        WHEN("retrieving static relic")
        {
            const auto staticRelic = reliquary->Static<StaticRelic>();

            THEN("structure has been satisfied")
            {
                REQUIRE(reliquary->Find<BasicShard>(staticRelic->ID()));
                REQUIRE(staticRelic->basicShard);
            }

            THEN("cannot destroy relic")
            {
                auto preDestroyRelicCount = reliquary->RelicSize();

                reliquary->Destroy(staticRelic);

                auto foundAgain = reliquary->Static<StaticRelic>();

                REQUIRE(foundAgain->ID() == staticRelic->ID());
                REQUIRE(foundAgain->basicShard == staticRelic->basicShard);
                REQUIRE(preDestroyRelicCount == reliquary->RelicSize());
            }
        }

        WHEN("retrieving static relic dynamically")
        {
            const auto staticRelic = reliquary->Static<StaticRelic>();
            const auto asDynamic = reliquary->Find<DynamicRelic>(staticRelic->ID());

            THEN("dynamic is empty")
            {
                REQUIRE(!asDynamic);
            }
        }

        WHEN("retrieving static relic as fixed")
        {
            const auto staticRelic = reliquary->Static<StaticRelic>();
            const auto asFixed = reliquary->Find<FixedRelic>(staticRelic->ID());

            THEN("fixed is empty")
            {
                REQUIRE(!asFixed);
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
            relics.push_back(reliquary->Create<DynamicRelic>());
            relics.back().Create<BasicShard>()->myValue = ::Chroma::ToString(i);
        }

        WHEN("deleting all but the last")
        {
            auto loop = relics.begin();
            while (loop != --relics.end())
            {
                reliquary->Destroy(*loop);
                loop = relics.erase(loop);
            }

            THEN("last relic still valid")
            {
                auto relic = relics[0];

                REQUIRE(relic.Has<BasicShard>() == true);

                auto shard = relic.Find<BasicShard>();
                REQUIRE(shard);
                REQUIRE(shard->myValue == ::Chroma::ToString(99));
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "custom factory relic", "[relic][factory]")
{
    GIVEN("most basic custom factory relic registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<MostBasicCustomFactoryRelic>()
            .Actualize();

        WHEN("creating relic")
        {
            const auto relic = reliquary->Create<MostBasicCustomFactoryRelic>();

            THEN("has value set from factory")
            {
                REQUIRE(relic->value == 999);
            }
        }
    }

    GIVEN("guarded custom factory relic registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Relic<GuardedCustomFactoryRelic>()
            .Actualize();

        WHEN("creating relic with 100 value")
        {
            const auto relic = reliquary->Create<GuardedCustomFactoryRelic>(100);

            THEN("relic was created")
            {
                REQUIRE(relic);
                REQUIRE(reliquary->Find<GuardedCustomFactoryRelic>(relic->ID()));
            }
        }

        WHEN("creating relic with 99 value")
        {
            const auto relic = reliquary->Create<GuardedCustomFactoryRelic>(99);

            THEN("relic was not created")
            {
                REQUIRE(!relic);
                REQUIRE(reliquary->Batch<GuardedCustomFactoryRelic>().IsEmpty());
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

        auto createdSignals = reliquary->Batch<Created<BasicTypedRelic>>();
        auto destroyingSignals = reliquary->Batch<Destroying<BasicTypedRelic>>();

        WHEN("creating relic")
        {
            const auto created = reliquary->Create<BasicTypedRelic>();

            THEN("signal is emitted")
            {
                REQUIRE(createdSignals.Size() == 1);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(*created);

                THEN("signal is emitted")
                {
                    REQUIRE(destroyingSignals.Size() == 1);
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

        auto staticRelic = reliquary->Static<StaticRelic>();
        auto dynamicRelic = reliquary->Create<DynamicRelic>();

        auto onParented = reliquary->Batch<RelicParented>();

        WHEN("parenting child to static parent")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->ParentRelicTo(staticRelic, dynamicRelic),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + Chroma::ToString(dynamicRelic.ID()) + ") " +
                        "was attempted to be parented to a static relic.")
                );
            }

            THEN("does not send signal")
            {
                REQUIRE(onParented.IsEmpty());
            }

            THEN("does not have parent")
            {
                REQUIRE(!dynamicRelic.Parent());
            }
        }

        WHEN("parenting static child to parent")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->ParentRelicTo(dynamicRelic, staticRelic),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + Chroma::ToString(staticRelic->ID()) + ") " +
                        "is static and cannot be parented to anything.")
                );
            }

            THEN("does not send signal")
            {
                REQUIRE(onParented.IsEmpty());
            }

            THEN("does not have parent")
            {
                REQUIRE(!staticRelic->Parent());
            }
        }
    }

    GIVEN("parent and child created")
    {
        auto reliquary = ReliquaryOrigin()
            .Shard<BasicShard>()
            .Actualize();

        auto parent = reliquary->Create<DynamicRelic>();
        parent.Create<BasicShard>();
        auto child = reliquary->Create<DynamicRelic>();
        child.Create<BasicShard>();

        auto onParented = reliquary->Batch<RelicParented>();

        WHEN("parenting child to parent")
        {
            reliquary->ParentRelicTo(parent, child);

            THEN("child has parent")
            {
                REQUIRE(child.Parent());
                REQUIRE(child.Parent()->ID() == parent.ID());
            }

            THEN("destroying parent also destroys child")
            {
                reliquary->Destroy(parent);

                REQUIRE(reliquary->RelicSize() == 0);
                REQUIRE(!reliquary->Find<BasicShard>(child.ID()));
            }

            WHEN("destroying child")
            {
                reliquary->Destroy(child);

                THEN("does not destroy parent")
                {
                    REQUIRE(reliquary->RelicSize() == 1);
                    REQUIRE(reliquary->Find<BasicShard>(parent.ID()));
                }

                THEN("destroying parent works")
                {
                    reliquary->Destroy(parent);

                    REQUIRE(reliquary->RelicSize() == 0);
                    REQUIRE(!reliquary->Find<BasicShard>(parent.ID()));
                }
            }

            WHEN("parenting child to parent again")
            {
                THEN("throws error")
                {
                    REQUIRE_THROWS_MATCHES
                    (
                        reliquary->ParentRelicTo(parent, child),
                        CannotParentRelic,
                        ::Catch::Matchers::Message(
                            "The relic with id("s + Chroma::ToString(child.ID()) + ") is already parented.")
                    );
                }
            }

            THEN("sends signal")
            {
                REQUIRE(!onParented.IsEmpty());

                auto signal = onParented.begin();

                REQUIRE(signal->parent.ID() == parent.ID());
                REQUIRE(signal->child.ID() == child.ID());
            }
        }

        WHEN("parenting child to nonexistent relic")
        {
            auto nonExistentRelic = RelicHandle();

            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->ParentRelicTo(nonExistentRelic, child),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + ::Chroma::ToString(nonExistentRelic.ID()) + ") is from a different Reliquary.")
                );
            }

            THEN("does not send signal")
            {
                REQUIRE(onParented.IsEmpty());
            }

            THEN("does not have parent")
            {
                REQUIRE(!child.Parent());
            }
        }

        WHEN("parenting nonexistent relic to parent")
        {
            auto nonExistentRelic = RelicHandle();

            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->ParentRelicTo(parent, nonExistentRelic),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + ::Chroma::ToString(nonExistentRelic.ID()) + ") is from a different Reliquary.")
                );
            }

            THEN("does not send signal")
            {
                REQUIRE(onParented.IsEmpty());
            }
        }

        WHEN("parenting relic to itself")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->ParentRelicTo(parent, parent),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + Chroma::ToString(parent.ID()) + ") " +
                        "was attempted to be parented to itself.")
                );
            }

            THEN("does not send signal")
            {
                REQUIRE(onParented.IsEmpty());
            }

            THEN("does not have parent")
            {
                REQUIRE(!parent.Parent());
            }
        }
    }

    GIVEN("parent from one reliquary and child from another")
    {
        auto origin = ReliquaryOrigin()
            .Shard<BasicShard>();

        auto parentReliquary = origin.Actualize();
        auto childReliquary = origin.Actualize();

        auto parent = parentReliquary->Create<DynamicRelic>();
        parent.Create<BasicShard>();
        auto child = childReliquary->Create<DynamicRelic>();
        child.Create<BasicShard>();

        WHEN("parenting child inside parent reliquary")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    parentReliquary->ParentRelicTo(parent, child),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + ::Chroma::ToString(child.ID()) + ") is from a different Reliquary.")
                );
            }
        }

        WHEN("parenting child inside child reliquary")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    childReliquary->ParentRelicTo(parent, child),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + ::Chroma::ToString(parent.ID()) + ") is from a different Reliquary.")
                );
            }
        }

        WHEN("parenting child inside irrelevant reliquary")
        {
            auto irrelevantReliquary = origin.Actualize();

            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    irrelevantReliquary->ParentRelicTo(parent, child),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The relic with id ("s + ::Chroma::ToString(parent.ID()) + ") is from a different Reliquary.")
                );
            }
        }
    }
}