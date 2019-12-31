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

void RelicTestsFixture::TypedRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void RelicTestsFixture::OpenTypedRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void RelicTestsFixture::GlobalRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void RelicTestsFixture::ShouldCreateRelic::Initialize(int value)
{
    this->value = value;
}

void RelicTestsFixture::InitializedRelic::PostConstruct(ShardTuple shards)
{
    basicShard = std::get<0>(shards);
}

void RelicTestsFixture::InitializedRelic::Initialize(int myValue)
{
    this->myValue = myValue;
}

void RelicTestsFixture::MovableOnlyRelic::PostConstruct(ShardTuple shards)
{
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

    const TypeName Traits<RelicTestsFixture::ShouldCreateRelic>::typeName =
        "ReliquaryTestsShouldCreateRelic";

    bool Traits<RelicTestsFixture::ShouldCreateRelic>::ShouldCreate(Reliquary& reliquary, int value)
    {
        return value >= 100;
    }

    const TypeName Traits<RelicTestsFixture::InitializedRelic>::typeName =
        "ReliquaryTestsInitializedRelic";

    const TypeName Traits<RelicTestsFixture::MovableOnlyRelic>::typeName =
        "ReliquaryTestsMovableOnlyRelic";
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
                REQUIRE(reliquary->Contains<OpenRelic>(openRelic->ID()));
            }

            WHEN("finding open relic")
            {
                auto found = Arca::LocalPtr<OpenRelic>(openRelic->ID(), *reliquary);

                THEN("found is same as created")
                {
                    REQUIRE(found->ID() == openRelic->ID());
                    REQUIRE(&found->Owner() == &openRelic->Owner());
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
                    REQUIRE(Arca::LocalPtr<Shard>(openRelic->ID(), *reliquary));
                    REQUIRE(reliquary->Contains<Shard>(openRelic->ID()));
                }

                THEN("relic has shard")
                {
                    REQUIRE(openRelic->Find<Shard>());
                    REQUIRE(openRelic->Find<Either<Shard>>());
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
                        REQUIRE(!Arca::LocalPtr<Shard>(openRelic->ID(), *reliquary));
                        REQUIRE(!reliquary->Contains<Shard>(openRelic->ID()));
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
                auto closedRelic = Arca::LocalPtr<ClosedRelic>(openRelic->ID(), *reliquary);

                THEN("closed relic is not found")
                {
                    REQUIRE(!closedRelic);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::LocalPtr<TypedRelic>(openRelic->ID(), *reliquary);

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
                REQUIRE(Arca::LocalPtr<Shard>(closedRelic->ID(), *reliquary));

                REQUIRE(closedRelic->Find<Shard>());
                REQUIRE(closedRelic->Contains<Shard>());
            }

            THEN("reliquary relic count increments by one")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicCount + 1));
            }

            THEN("reliquary contains relic")
            {
                REQUIRE(reliquary->Contains<ClosedRelic>(closedRelic->ID()));
            }

            WHEN("finding open relic")
            {
                auto found = Arca::LocalPtr<ClosedRelic>(closedRelic->ID(), *reliquary);

                THEN("found is same as created")
                {
                    REQUIRE(found->ID() == closedRelic->ID());
                    REQUIRE(&found->Owner() == &closedRelic->Owner());
                }
            }

            WHEN("destroying relic")
            {
                auto preDestroyRelicCount = reliquary->RelicSize();

                auto id = closedRelic->ID();
                reliquary->Destroy(AsHandle(*closedRelic));

                THEN("finding relic returns empty")
                {
                    REQUIRE(!Arca::LocalPtr<ClosedRelic>(id, *reliquary));
                }

                THEN("destroying again does not throw")
                {
                    REQUIRE_NOTHROW(reliquary->Destroy(AsHandle(*closedRelic)));
                }

                THEN("reliquary relic count decrements by one")
                {
                    REQUIRE(reliquary->RelicSize() == (preDestroyRelicCount - 1));
                }
            }

            WHEN("retrieving as open")
            {
                const auto asOpen = Arca::LocalPtr<OpenRelic>(closedRelic->ID(), *reliquary);

                THEN("open is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::LocalPtr<TypedRelic>(closedRelic->ID(), *reliquary);

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
                REQUIRE(reliquary->Contains<TypedRelic>(typedRelic->ID()));
            }

            WHEN("finding typed relic")
            {
                auto found = Arca::LocalPtr<TypedRelic>(typedRelic->ID(), *reliquary);

                THEN("found is same as created")
                {
                    REQUIRE(found->ID() == typedRelic->ID());
                    REQUIRE(&found->Owner() == &typedRelic->Owner());
                }
            }

            THEN("has shard")
            {
                REQUIRE(typedRelic->basicShard);
            }

            WHEN("retrieving as open")
            {
                const auto asOpen = Arca::LocalPtr<OpenRelic>(typedRelic->ID(), *reliquary);

                THEN("typed is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as closed")
            {
                auto closedRelic = Arca::LocalPtr<ClosedRelic>(typedRelic->ID(), *reliquary);

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

        std::vector<OpenRelic*> relics;
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
                reliquary->Destroy(AsHandle(**loop));
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

        WHEN("clearing")
        {
            reliquary->Clear<OpenRelic>();

            THEN("reliquary is empty")
            {
                REQUIRE(reliquary->RelicSize() == 0);
                REQUIRE(reliquary->ShardSize() == 0);
            }
        }

        WHEN("clearing by type")
        {
            reliquary->Clear(TypeFor<OpenRelic>());

            THEN("reliquary is empty")
            {
                REQUIRE(reliquary->RelicSize() == 0);
                REQUIRE(reliquary->ShardSize() == 0);
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "custom should create relic", "[relic][shouldcreate]")
{
    GIVEN("should create relic registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<ShouldCreateRelic>()
            .Actualize();

        WHEN("creating relic with 100 value")
        {
            const auto relic = reliquary->Create<ShouldCreateRelic>(100);

            THEN("relic was created")
            {
                REQUIRE(relic);
                REQUIRE(Arca::LocalPtr<ShouldCreateRelic>(relic->ID(), *reliquary));
            }
        }

        WHEN("creating relic with 99 value")
        {
            const auto relic = reliquary->Create<ShouldCreateRelic>(99);

            THEN("relic was not created")
            {
                REQUIRE(!relic);
                REQUIRE(reliquary->Batch<ShouldCreateRelic>().IsEmpty());
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

        auto createdSignals = reliquary->Batch<Created>();
        auto destroyingSignals = reliquary->Batch<Destroying>();

        WHEN("creating open relic")
        {
            const auto created = reliquary->Create<OpenRelic>();

            THEN("signal is emitted for relic and shard")
            {
                REQUIRE(createdSignals.Size() == 1);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(AsHandle(*created));

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingSignals.Size() == 1);
                }
            }

            WHEN("clearing")
            {
                reliquary->Clear<OpenRelic>();

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingSignals.Size() == 1);
                }
            }

            WHEN("clearing by type")
            {
                reliquary->Clear(TypeFor<OpenRelic>());

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingSignals.Size() == 1);
                }
            }
        }

        WHEN("creating closed relic")
        {
            const auto created = reliquary->CreateWith<ClosedRelic>(RelicStructure { TypeFor<Shard>() });

            THEN("signal is emitted for relic and shard")
            {
                REQUIRE(createdSignals.Size() == 2);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(AsHandle(*created));

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingSignals.Size() == 2);
                }
            }

            WHEN("clearing")
            {
                reliquary->Clear<ClosedRelic>();

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingSignals.Size() == 2);
                }
            }

            WHEN("clearing by type")
            {
                reliquary->Clear(TypeFor<ClosedRelic>());

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingSignals.Size() == 2);
                }
            }
        }

        WHEN("creating typed relic")
        {
            const auto created = reliquary->Create<TypedRelic>();

            THEN("signal is emitted for relic and shard")
            {
                REQUIRE(createdSignals.Size() == 2);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(AsHandle(*created));

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingSignals.Size() == 2);
                }
            }

            WHEN("clearing")
            {
                reliquary->Clear<TypedRelic>();

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingSignals.Size() == 2);
                }
            }

            WHEN("clearing by type")
            {
                reliquary->Clear(TypeFor<TypedRelic>());

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(destroyingSignals.Size() == 2);
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
                REQUIRE(Arca::LocalPtr<OtherShard>(relic->ID(), *reliquary));
            }

            WHEN("finding by either")
            {
                auto found = relic->Find<Either<OtherShard>>();

                THEN("is same as created")
                {
                    REQUIRE(&*found == &*shard);
                }
            }

            WHEN("destroying dynamic shard")
            {
                relic->Destroy<OtherShard>();

                THEN("reliquary has destroyed shard")
                {
                    REQUIRE(reliquary->ShardSize() == 1);
                    REQUIRE(!Arca::LocalPtr<OtherShard>(relic->ID(), *reliquary));
                }
            }

            WHEN("destroying shard via handle")
            {
                reliquary->Destroy(AsHandle<OtherShard>(relic->ID(), relic->Owner()));

                THEN("reliquary has destroyed shard")
                {
                    REQUIRE(reliquary->ShardSize() == 1);
                    REQUIRE(!Arca::LocalPtr<OtherShard>(relic->ID(), *reliquary));
                }
            }

            WHEN("creating same shard type with const")
            {
                THEN("throws error")
                {
                    REQUIRE_THROWS_AS(relic->Create<const OtherShard>(), CannotCreate);
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

        auto globalRelic = Arca::GlobalPtr<GlobalRelic>(*reliquary);

        auto onParented = reliquary->Batch<RelicParented>();

        WHEN("parenting child to global parent")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->CreateChild<OpenRelic>(AsHandle(*globalRelic)),
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
            auto child = reliquary->CreateChild<OpenRelic>(AsHandle(*parent));
            child->Create<Shard>();

            THEN("child has parent")
            {
                REQUIRE(child->Parent());
                REQUIRE(child->Parent()->ID() == parent->ID());
            }

            THEN("destroying parent also destroys child")
            {
                reliquary->Destroy(AsHandle(*parent));

                REQUIRE(reliquary->RelicSize() == 0);
                REQUIRE(!Arca::LocalPtr<Shard>(child->ID(), *reliquary));
            }

            WHEN("destroying child")
            {
                reliquary->Destroy(AsHandle(*child));

                THEN("does not destroy parent")
                {
                    REQUIRE(reliquary->RelicSize() == 1);
                    REQUIRE(Arca::LocalPtr<Shard>(parent->ID(), *reliquary));
                }

                THEN("destroying parent works")
                {
                    reliquary->Destroy(AsHandle(*parent));

                    REQUIRE(reliquary->RelicSize() == 0);
                    REQUIRE(!Arca::LocalPtr<Shard>(parent->ID(), *reliquary));
                }
            }

            THEN("sends signal")
            {
                REQUIRE(!onParented.IsEmpty());

                auto signal = onParented.begin();

                REQUIRE(signal->parent.ID() == parent->ID());
                REQUIRE(signal->child.ID() == child->ID());
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
                    childReliquary->CreateChild<OpenRelic>(AsHandle(*parent)),
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
                    irrelevantReliquary->CreateChild<OpenRelic>(AsHandle(*parent)),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "The parent relic is from a different Reliquary.")
                );
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "relic initialization", "[relic]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<InitializedRelic>()
            .Type<Shard>()
            .Actualize();

        WHEN("creating initialized relic")
        {
            auto myValue = dataGeneration.Random<int>();
            auto relic = reliquary->Create<InitializedRelic>(myValue);

            THEN("has value")
            {
                REQUIRE(relic->myValue == myValue);
            }

            THEN("has shard")
            {
                REQUIRE(relic->basicShard);
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "relic moving only", "[relic]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<MovableOnlyRelic>()
            .Type<Shard>()
            .Actualize();

        WHEN("creating movable only relic")
        {
            auto relic = reliquary->Create<MovableOnlyRelic>();

            auto myValue = dataGeneration.Random<int>();
            relic->myValue = myValue;

            THEN("has value")
            {
                REQUIRE(relic->myValue == myValue);
            }

            THEN("has shard")
            {
                REQUIRE(relic->basicShard);
            }
        }
    }
}