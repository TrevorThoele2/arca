#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "RelicTests.h"

#include <Arca/RelicParented.h>

#include <Chroma/StringUtility.h>

RelicTestsFixture::Shard::Shard(std::string myValue) : myValue(std::move(myValue))
{}

RelicTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

void RelicTestsFixture::TypedRelic::InitializeImplementation()
{
    auto shards = ExtractShards();
    basicShard = std::get<0>(shards);
}

void RelicTestsFixture::OpenTypedRelic::InitializeImplementation()
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
    const TypeName Traits<RelicTestsFixture::Shard>::typeName =
        "RelicTestsShard";

    const TypeName Traits<RelicTestsFixture::OtherShard>::typeName =
        "RelicTestsOtherShard";

    const TypeName Traits<RelicTestsFixture::TypedRelic>::typeName =
        "RelicTestsTypedRelic";

    const TypeName Traits<RelicTestsFixture::OpenTypedRelic>::typeName =
        "RelicTestsOpenTypedRelic";

    const TypeName Traits<RelicTestsFixture::GlobalRelic>::typeName =
        "RelicTestsGlobalRelic";

    const TypeName Traits<RelicTestsFixture::MostBasicCustomFactoryRelic>::typeName =
        "RelicTestsMostBasicCustomFactoryRelic";

    std::optional<RelicTestsFixture::MostBasicCustomFactoryRelic>
        Traits<RelicTestsFixture::MostBasicCustomFactoryRelic>::Factory(Reliquary& reliquary)
    {
        RelicTestsFixture::MostBasicCustomFactoryRelic relic;
        relic.value = 999;
        return relic;
    }

    const TypeName Traits<RelicTestsFixture::GuardedCustomFactoryRelic>::typeName =
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
            .Type<Shard>()
            .Type<OtherShard>()
            .Type<TypedRelic>()
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
                auto shard = openRelic->Create<Shard>();

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
                    REQUIRE(reliquary->Find<Shard>(openRelic.ID()));
                    REQUIRE(reliquary->Contains<Shard>(openRelic.ID()));
                }

                THEN("relic has shard")
                {
                    REQUIRE(openRelic->Find<Shard>());
                    REQUIRE(openRelic->Contains<Shard>());
                }

                WHEN("destroying shard")
                {
                    openRelic->Destroy<Shard>();

                    THEN("reliquary loses a shard")
                    {
                        REQUIRE(reliquary->ShardSize() == preCreateRelicSize);
                    }

                    THEN("reliquary does not have shard")
                    {
                        REQUIRE(!reliquary->Find<Shard>(openRelic.ID()));
                        REQUIRE(!reliquary->Contains<Shard>(openRelic.ID()));
                    }

                    THEN("relic does not have shard")
                    {
                        REQUIRE(!openRelic->Find<Shard>());
                        REQUIRE(!openRelic->Contains<Shard>());
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
                const auto asTyped = reliquary->Find<TypedRelic>(openRelic.ID());

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }

        WHEN("creating closed relic with valid structure")
        {
            auto preCreateRelicCount = reliquary->RelicSize();
            auto closedRelic = reliquary->CreateWith<ClosedRelic>(RelicStructure{ TypeFor<Shard>() });

            THEN("structure has been satisfied")
            {
                REQUIRE(reliquary->Find<Shard>(closedRelic.ID()));

                REQUIRE(closedRelic->Find<Shard>());
                REQUIRE(closedRelic->Contains<Shard>());
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
                const auto asTyped = reliquary->Find<TypedRelic>(closedRelic.ID());

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }

        WHEN("creating typed relic")
        {
            auto preCreateRelicSize = reliquary->RelicSize();
            auto typedRelic = reliquary->Create<TypedRelic>();

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
                REQUIRE(reliquary->Contains<TypedRelic>(typedRelic.ID()));
            }

            WHEN("finding typed relic")
            {
                auto found = reliquary->Find<TypedRelic>(typedRelic.ID());

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
            .Type<Shard>()
            .Type<OtherShard>()
            .Actualize();

        std::vector<Ptr<OpenRelic>> relics;
        for (size_t i = 0; i < 100; ++i)
        {
            relics.push_back(reliquary->Create<OpenRelic>());
            relics.back()->Create<Shard>()->myValue = ::Chroma::ToString(i);
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

                REQUIRE(relic->Contains<Shard>() == true);

                auto shard = relic->Find<Shard>();
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
            .Type<TypedRelic>()
            .Type<Shard>()
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
            const auto created = reliquary->CreateWith<ClosedRelic>(RelicStructure { TypeFor<Shard>() });

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
            const auto created = reliquary->Create<TypedRelic>();

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

SCENARIO_METHOD(RelicTestsFixture, "open typed relic", "[relic][open]")
{
    GIVEN("created open typed relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<OpenTypedRelic>()
            .Type<Shard>()
            .Type<OtherShard>()
            .Actualize();

        auto relic = reliquary->Create<OpenTypedRelic>();

        WHEN("default created")
        {
            THEN("has all requested shards")
            {
                REQUIRE(relic->basicShard);
            }
        }

        WHEN("creating dynamic shard")
        {
            auto shard = relic->Create<OtherShard>();

            THEN("returned is valid")
            {
                REQUIRE(shard);
            }

            THEN("reliquary created shard")
            {
                REQUIRE(reliquary->ShardSize() == 2);
                REQUIRE(reliquary->Find<OtherShard>(relic->ID()));
            }

            WHEN("destroying dynamic shard")
            {
                relic->Destroy<OtherShard>();

                THEN("returned is now invalid")
                {
                    REQUIRE(!shard);
                }

                THEN("reliquary has destroyed shard")
                {
                    REQUIRE(reliquary->ShardSize() == 1);
                    REQUIRE(!reliquary->Find<OtherShard>(relic->ID()));
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
            .Type<Shard>()
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
            .Type<Shard>()
            .Actualize();

        auto parent = reliquary->Create<OpenRelic>();
        parent->Create<Shard>();

        auto onParented = reliquary->Batch<RelicParented>();

        WHEN("created child")
        {
            auto child = reliquary->CreateChild<OpenRelic>(parent);
            child->Create<Shard>();

            THEN("child has parent")
            {
                REQUIRE(child->Parent());
                REQUIRE(child->Parent()->ID() == parent.ID());
            }

            THEN("destroying parent also destroys child")
            {
                reliquary->Destroy(parent);

                REQUIRE(reliquary->RelicSize() == 0);
                REQUIRE(!reliquary->Find<Shard>(child.ID()));
            }

            WHEN("destroying child")
            {
                reliquary->Destroy(child);

                THEN("does not destroy parent")
                {
                    REQUIRE(reliquary->RelicSize() == 1);
                    REQUIRE(reliquary->Find<Shard>(parent.ID()));
                }

                THEN("destroying parent works")
                {
                    reliquary->Destroy(parent);

                    REQUIRE(reliquary->RelicSize() == 0);
                    REQUIRE(!reliquary->Find<Shard>(parent.ID()));
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
            .Type<Shard>();

        auto parentReliquary = origin.Actualize();
        auto childReliquary = origin.Actualize();

        auto parent = parentReliquary->Create<OpenRelic>();
        parent->Create<Shard>();

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
            .Type<TypedRelic>()
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