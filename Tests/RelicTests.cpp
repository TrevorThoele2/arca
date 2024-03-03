#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "RelicTests.h"

#include <Arca/RelicParented.h>

#include <Chroma/StringUtility.h>

RelicTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

RelicTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

void RelicTestsFixture::BasicTypedRelic::InitializeImplementation()
{
    auto shards = ExtractShards();
    basicShard = std::get<0>(shards);
}

void RelicTestsFixture::GlobalRelic::InitializeImplementation()
{
    auto shards = ExtractShards();
    basicShard = std::get<0>(shards);
}

namespace Arca
{
    const TypeHandleName Traits<RelicTestsFixture::BasicShard>::typeName =
        "RelicTestsBasicShard";

    const TypeHandleName Traits<RelicTestsFixture::OtherShard>::typeName =
        "ReliquaryTestsOtherShard";

    const TypeHandleName Traits<RelicTestsFixture::BasicTypedRelic>::typeName =
        "ReliquaryTestsBasicTypedRelic";

    const TypeHandleName Traits<RelicTestsFixture::GlobalRelic>::typeName =
        "ReliquaryTestsGlobalRelic";

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
            .Type<BasicShard>()
            .Type<OtherShard>()
            .Type<BasicTypedRelic>()
            .Type<GlobalRelic>()
            .Actualize();

        WHEN("creating open relic")
        {
            auto preCreateRelicSize = reliquary->RelicSize();
            auto openRelic = reliquary->Create<OpenRelic>();

            THEN("does not have parent")
            {
                REQUIRE(!openRelic->Parent());
            }

            THEN("reliquary has one more relic")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicSize + 1));
            }

            THEN("reliquary contains relic")
            {
                REQUIRE(reliquary->Contains<OpenRelic>(openRelic.ID()));
            }

            WHEN("finding open relic")
            {
                auto found = reliquary->Find<OpenRelic>(openRelic.ID());

                THEN("found is same as created")
                {
                    REQUIRE(found->ID() == openRelic.ID());
                    REQUIRE(&found->Owner() == &openRelic.Owner());
                }
            }

            WHEN("creating shard")
            {
                auto preCreateShardSize = reliquary->ShardSize();
                auto shard = openRelic->Create<BasicShard>();

                THEN("reliquary has one more shard")
                {
                    REQUIRE(reliquary->ShardSize() == (preCreateShardSize + 1));
                }

                THEN("shard is returned")
                {
                    REQUIRE(shard);
                }

                THEN("reliquary has shard")
                {
                    REQUIRE(reliquary->Find<BasicShard>(openRelic.ID()));
                    REQUIRE(reliquary->Contains<BasicShard>(openRelic.ID()));
                }

                THEN("relic has shard")
                {
                    REQUIRE(openRelic->Find<BasicShard>());
                    REQUIRE(openRelic->Contains<BasicShard>());
                }

                WHEN("destroying shard")
                {
                    openRelic->Destroy<BasicShard>();

                    THEN("reliquary loses a shard")
                    {
                        REQUIRE(reliquary->ShardSize() == preCreateRelicSize);
                    }

                    THEN("reliquary does not have shard")
                    {
                        REQUIRE(!reliquary->Find<BasicShard>(openRelic.ID()));
                        REQUIRE(!reliquary->Contains<BasicShard>(openRelic.ID()));
                    }

                    THEN("relic does not have shard")
                    {
                        REQUIRE(!openRelic->Find<BasicShard>());
                        REQUIRE(!openRelic->Contains<BasicShard>());
                    }
                }
            }

            WHEN("retrieving as closed")
            {
                auto closedRelic = reliquary->Find<ClosedRelic>(openRelic.ID());

                THEN("closed relic is not found")
                {
                    REQUIRE(!closedRelic);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = reliquary->Find<BasicTypedRelic>(openRelic.ID());

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }

        WHEN("creating closed relic with valid structure")
        {
            auto preCreateRelicCount = reliquary->RelicSize();
            auto closedRelic = reliquary->CreateWith<ClosedRelic>(RelicStructure{ TypeHandleFor<BasicShard>() });

            THEN("structure has been satisfied")
            {
                REQUIRE(reliquary->Find<BasicShard>(closedRelic.ID()));

                REQUIRE(closedRelic->Find<BasicShard>());
                REQUIRE(closedRelic->Contains<BasicShard>());
            }

            THEN("reliquary relic count increments by one")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicCount + 1));
            }

            THEN("reliquary contains relic")
            {
                REQUIRE(reliquary->Contains<ClosedRelic>(closedRelic.ID()));
            }

            WHEN("finding open relic")
            {
                auto found = reliquary->Find<ClosedRelic>(closedRelic.ID());

                THEN("found is same as created")
                {
                    REQUIRE(found->ID() == closedRelic.ID());
                    REQUIRE(&found->Owner() == &closedRelic.Owner());
                }
            }

            WHEN("destroying relic")
            {
                auto preDestroyRelicCount = reliquary->RelicSize();

                auto id = closedRelic.ID();
                reliquary->Destroy(closedRelic);

                THEN("finding relic returns empty")
                {
                    REQUIRE(!reliquary->Find<ClosedRelic>(id));
                }

                THEN("destroying again does not throw")
                {
                    REQUIRE_NOTHROW(reliquary->Destroy(closedRelic));
                }

                THEN("reliquary relic count decrements by one")
                {
                    REQUIRE(reliquary->RelicSize() == (preDestroyRelicCount - 1));
                }
            }

            WHEN("retrieving as open")
            {
                const auto asOpen = reliquary->Find<OpenRelic>(closedRelic.ID());

                THEN("open is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = reliquary->Find<BasicTypedRelic>(closedRelic.ID());

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }

        WHEN("creating typed relic")
        {
            auto preCreateRelicSize = reliquary->RelicSize();
            auto typedRelic = reliquary->Create<BasicTypedRelic>();

            THEN("does not have parent")
            {
                REQUIRE(!typedRelic->Parent());
            }

            THEN("reliquary has one more relic")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicSize + 1));
            }

            THEN("reliquary contains relic")
            {
                REQUIRE(reliquary->Contains<BasicTypedRelic>(typedRelic.ID()));
            }

            WHEN("finding typed relic")
            {
                auto found = reliquary->Find<BasicTypedRelic>(typedRelic.ID());

                THEN("found is same as created")
                {
                    REQUIRE(found->ID() == typedRelic.ID());
                    REQUIRE(&found->Owner() == &typedRelic.Owner());
                }
            }

            THEN("has shard")
            {
                REQUIRE(typedRelic->basicShard);
            }

            WHEN("retrieving as open")
            {
                const auto asOpen = reliquary->Find<OpenRelic>(typedRelic.ID());

                THEN("typed is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as closed")
            {
                auto closedRelic = reliquary->Find<ClosedRelic>(typedRelic.ID());

                THEN("closed relic is not found")
                {
                    REQUIRE(!closedRelic);
                }
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "many relics", "[relic]")
{
    GIVEN("many open relics")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Type<OtherShard>()
            .Actualize();

        std::vector<Ptr<OpenRelic>> relics;
        for (size_t i = 0; i < 100; ++i)
        {
            relics.push_back(reliquary->Create<OpenRelic>());
            relics.back()->Create<BasicShard>()->myValue = ::Chroma::ToString(i);
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

                REQUIRE(relic->Contains<BasicShard>() == true);

                auto shard = relic->Find<BasicShard>();
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
            .Type<MostBasicCustomFactoryRelic>()
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
            .Type<GuardedCustomFactoryRelic>()
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
            .Type<BasicTypedRelic>()
            .Type<BasicShard>()
            .Actualize();

        auto createdRelicSignals = reliquary->Batch<Created>();
        auto destroyingRelicSignals = reliquary->Batch<Destroying>();

        WHEN("creating open relic")
        {
            const auto created = reliquary->Create<OpenRelic>();

            THEN("signal is emitted for relic")
            {
                REQUIRE(createdRelicSignals.Size() == 1);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(created);

                THEN("signal is emitted for relic")
                {
                    REQUIRE(destroyingRelicSignals.Size() == 1);
                }
            }
        }

        WHEN("creating closed relic")
        {
            const auto created = reliquary->CreateWith<ClosedRelic>(RelicStructure { TypeHandleFor<BasicShard>() });

            THEN("signal is emitted for relic")
            {
                REQUIRE(createdRelicSignals.Size() == 2);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(created);

                THEN("signal is emitted for relic")
                {
                    REQUIRE(destroyingRelicSignals.Size() == 2);
                }
            }
        }

        WHEN("creating typed relic")
        {
            const auto created = reliquary->Create<BasicTypedRelic>();

            THEN("signal is emitted for relic and shard")
            {
                REQUIRE(createdRelicSignals.Size() == 2);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(*created);

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingRelicSignals.Size() == 2);
                }
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "relic parenting", "[relic][parenting]")
{
    GIVEN("global relic and open relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<GlobalRelic>()
            .Type<BasicShard>()
            .Actualize();

        auto globalRelic = reliquary->Find<GlobalRelic>();

        auto onParented = reliquary->Batch<RelicParented>();

        WHEN("parenting child to global parent")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->CreateChild<OpenRelic>(globalRelic),
                    CannotParentRelic,
                    ::Catch::Matchers::Message("Attempted to parent to a global relic.")
                );
            }

            THEN("does not send signal")
            {
                REQUIRE(onParented.IsEmpty());
            }
        }
    }

    GIVEN("parent and child created")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Actualize();

        auto parent = reliquary->Create<OpenRelic>();
        parent->Create<BasicShard>();

        auto onParented = reliquary->Batch<RelicParented>();

        WHEN("created child")
        {
            auto child = reliquary->CreateChild<OpenRelic>(parent);
            child->Create<BasicShard>();

            THEN("child has parent")
            {
                REQUIRE(child->Parent());
                REQUIRE(child->Parent()->ID() == parent.ID());
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

            THEN("sends signal")
            {
                REQUIRE(!onParented.IsEmpty());

                auto signal = onParented.begin();

                REQUIRE(signal->parent.ID() == parent.ID());
                REQUIRE(signal->child.ID() == child.ID());
            }
        }

        WHEN("creating child on nonexistent relic")
        {
            auto nonExistentRelic = Handle();

            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->CreateChild<OpenRelic>(nonExistentRelic),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The parent relic is from a different Reliquary.")
                );
            }

            THEN("does not send signal")
            {
                REQUIRE(onParented.IsEmpty());
            }
        }
    }

    GIVEN("parent from one reliquary and child from another")
    {
        auto origin = ReliquaryOrigin()
            .Type<BasicShard>();

        auto parentReliquary = origin.Actualize();
        auto childReliquary = origin.Actualize();

        auto parent = parentReliquary->Create<OpenRelic>();
        parent->Create<BasicShard>();

        WHEN("parenting child inside child reliquary")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    childReliquary->CreateChild<OpenRelic>(parent),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The parent relic is from a different Reliquary.")
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
                    irrelevantReliquary->CreateChild<OpenRelic>(parent),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The parent relic is from a different Reliquary.")
                );
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "references to relics stay empty when relic placed later", "[reliquary][relic]")
{
    GIVEN("all registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<BasicTypedRelic>()
            .Actualize();

        WHEN("generating relic list, deleting half then recreating half of list")
        {
            std::vector<Ptr<OpenRelic>> everyOtherRelic;
            auto pickRelic = false;
            for (auto i = 0; i < 100; ++i)
            {
                auto relic = reliquary->Create<OpenRelic>();
                if (pickRelic)
                    everyOtherRelic.push_back(relic);
                pickRelic = !pickRelic;
            }

            for (auto& relic : everyOtherRelic)
                reliquary->Destroy(relic);

            for (auto i = 0; i < 50; ++i)
                reliquary->Create<OpenRelic>();

            THEN("relic references to previous relics do not point to anything")
            {
                REQUIRE(std::all_of(
                    everyOtherRelic.begin(),
                    everyOtherRelic.end(),
                    [](const Ptr<OpenRelic>& entry)
                    {
                        return !static_cast<bool>(entry);
                    }));
            }
        }
    }
}