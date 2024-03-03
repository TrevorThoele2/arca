#include <catch.hpp>
using namespace std::string_literals;

#include "RelicTests.h"

#include <Arca/RelicParented.h>

#include <Chroma/StringUtility.h>

RelicTestsFixture::Shard::Shard(std::string myValue) : myValue(myValue)
{}

RelicTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

RelicTestsFixture::TypedClosedRelic::TypedClosedRelic(Init init)
    : ClosedTypedRelic(init)
{
    basicShard = FindOrCreate<Shard>();
}

RelicTestsFixture::TypedOpenRelic::TypedOpenRelic(Init init)
    : OpenTypedRelic(init)
{
    basicShard = FindOrCreate<Shard>();
}

RelicTestsFixture::GlobalRelic::GlobalRelic(Init init)
    : ClosedTypedRelic(init)
{
    basicShard = FindOrCreate<Shard>();
}

RelicTestsFixture::ShouldCreateRelic::ShouldCreateRelic(Init init, int value)
    : ClosedTypedRelic(init), value(value)
{}

RelicTestsFixture::InitializedRelic::InitializedRelic(Init init)
    : ClosedTypedRelic(init)
{
    basicShard = FindOrCreate<Shard>();
}

RelicTestsFixture::InitializedRelic::InitializedRelic(Init init, int value)
    : ClosedTypedRelic(init), myValue(value)
{
    basicShard = Create<Shard>();
}

RelicTestsFixture::MovableOnlyRelic::MovableOnlyRelic(Init init)
    : ClosedTypedRelic(init)
{
    basicShard = FindOrCreate<Shard>();
}

RelicTestsFixture::MovableOnlyRelic::MovableOnlyRelic(Init init, int myInt)
    : ClosedTypedRelic(init), myValue(myInt)
{
    basicShard = Create<Shard>();
}

RelicTestsFixture::RelicConstructedFromMovedValue::RelicConstructedFromMovedValue(
    Init init, std::unique_ptr<int>&& myInt)
    :
    ClosedTypedRelic(init), myInt(std::move(myInt))
{}

namespace Arca
{
    bool Traits<RelicTestsFixture::ShouldCreateRelic>::ShouldCreate(Reliquary& reliquary, int value)
    {
        return value >= 100;
    }
}

SCENARIO_METHOD(RelicTestsFixture, "relic", "[relic]")
{
    GIVEN("all types registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Register<OtherShard>()
            .Register<TypedClosedRelic>()
            .Register<GlobalRelic>()
            .Actualize();

        WHEN("creating open relic")
        {
            auto preCreateRelicSize = reliquary->RelicSize();
            auto openRelic = reliquary->Do<Create<OpenRelic>>();

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
                auto found = Arca::Index<OpenRelic>(openRelic->ID(), *reliquary);

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
                    REQUIRE(Arca::Index<Shard>(openRelic->ID(), *reliquary));
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
                        REQUIRE(!Arca::Index<Shard>(openRelic->ID(), *reliquary));
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
                auto closedRelic = Arca::Index<ClosedRelic>(openRelic->ID(), *reliquary);

                THEN("closed relic is not found")
                {
                    REQUIRE(!closedRelic);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::Index<TypedClosedRelic>(openRelic->ID(), *reliquary);

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }

        WHEN("creating closed relic with valid structure")
        {
            auto preCreateRelicCount = reliquary->RelicSize();
            auto closedRelic = reliquary->Do<CreateWith<ClosedRelic>>(RelicStructure{ TypeFor<Shard>() });

            THEN("structure has been satisfied")
            {
                REQUIRE(Arca::Index<Shard>(closedRelic->ID(), *reliquary));

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
                auto found = Arca::Index<ClosedRelic>(closedRelic->ID(), *reliquary);

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
                auto handle = AsHandle(*closedRelic);
                reliquary->Destroy(handle);

                THEN("finding relic returns empty")
                {
                    REQUIRE(!Arca::Index<ClosedRelic>(id, *reliquary));
                }

                THEN("destroying again does not throw")
                {
                    REQUIRE_NOTHROW(reliquary->Destroy(handle));
                }

                THEN("reliquary relic count decrements by one")
                {
                    REQUIRE(reliquary->RelicSize() == (preDestroyRelicCount - 1));
                }
            }

            WHEN("retrieving as open")
            {
                const auto asOpen = Arca::Index<OpenRelic>(closedRelic->ID(), *reliquary);

                THEN("open is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::Index<TypedClosedRelic>(closedRelic->ID(), *reliquary);

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }

        WHEN("creating typed relic")
        {
            auto preCreateRelicSize = reliquary->RelicSize();
            auto typedRelic = reliquary->Do<Create<TypedClosedRelic>>();

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
                REQUIRE(reliquary->Contains<TypedClosedRelic>(typedRelic->ID()));
            }

            WHEN("finding typed relic")
            {
                auto found = Arca::Index<TypedClosedRelic>(typedRelic->ID(), *reliquary);

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
                const auto asOpen = Arca::Index<OpenRelic>(typedRelic->ID(), *reliquary);

                THEN("typed is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as closed")
            {
                auto closedRelic = Arca::Index<ClosedRelic>(typedRelic->ID(), *reliquary);

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
            .Register<Shard>()
            .Register<OtherShard>()
            .Actualize();

        std::vector<Index<OpenRelic>> relics;
        for (size_t i = 0; i < 100; ++i)
        {
            relics.push_back(reliquary->Do<Create<OpenRelic>>());
            relics.back()->Create<Shard>(::Chroma::ToString(i));
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
            reliquary->Do<Clear>(Chroma::TypeIdentity<OpenRelic>{});

            THEN("reliquary is empty")
            {
                REQUIRE(reliquary->RelicSize() == 0);
                REQUIRE(reliquary->ShardSize() == 0);
            }
        }

        WHEN("clearing by type")
        {
            reliquary->Do<Clear>(TypeFor<OpenRelic>());

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
            .Register<ShouldCreateRelic>()
            .Actualize();

        WHEN("creating relic with 100 value")
        {
            const auto relic = reliquary->Do<Create<ShouldCreateRelic>>(100);

            THEN("relic was created")
            {
                REQUIRE(relic);
                REQUIRE(Arca::Index<ShouldCreateRelic>(relic->ID(), *reliquary));
            }
        }

        WHEN("creating relic with 99 value")
        {
            const auto relic = reliquary->Do<Create<ShouldCreateRelic>>(99);

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
            .Register<TypedClosedRelic>()
            .Register<Shard>()
            .Actualize();

        auto genericCreatedSignals = reliquary->Batch<Created>();
        auto genericDestroyingSignals = reliquary->Batch<Destroying>();

        WHEN("creating open relic")
        {
            auto knownCreatedSignals = reliquary->Batch<CreatedKnown<OpenRelic>>();
            auto knownDestroyingSignals = reliquary->Batch<DestroyingKnown<OpenRelic>>();

            const auto created = reliquary->Do<Create<OpenRelic>>();
            const auto createdHandle = AsHandle(*created);

            THEN("signal is emitted for relic")
            {
                REQUIRE(genericCreatedSignals.Size() == 1);
                REQUIRE(genericCreatedSignals.begin()->handle == createdHandle);
                REQUIRE(knownCreatedSignals.Size() == 1);
                REQUIRE(knownCreatedSignals.begin()->reference == created);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(AsHandle(*created));

                THEN("signal is emitted for relic")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 1);
                    REQUIRE(genericDestroyingSignals.begin()->handle == createdHandle);
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->reference == created);
                }
            }

            WHEN("clearing")
            {
                reliquary->Do<Clear>(Chroma::TypeIdentity<OpenRelic>{});

                THEN("signal is emitted for relic")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 1);
                    REQUIRE(genericDestroyingSignals.begin()->handle == createdHandle);
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->reference == created);
                }
            }

            WHEN("clearing by type")
            {
                reliquary->Do<Clear>(TypeFor<OpenRelic>());

                THEN("signal is emitted for relic")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 1);
                    REQUIRE(genericDestroyingSignals.begin()->handle == createdHandle);
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->reference == created);
                }
            }
        }

        WHEN("creating closed relic")
        {
            auto knownCreatedSignals = reliquary->Batch<CreatedKnown<ClosedRelic>>();
            auto knownDestroyingSignals = reliquary->Batch<DestroyingKnown<ClosedRelic>>();

            const auto created = reliquary->Do<CreateWith<ClosedRelic>>(RelicStructure { TypeFor<Shard>() });
            const auto createdHandle = AsHandle(*created);

            THEN("generic signal is emitted for relic and shard")
            {
                REQUIRE(genericCreatedSignals.Size() == 2);
                REQUIRE(std::any_of(
                    genericCreatedSignals.begin(),
                    genericCreatedSignals.end(),
                    [createdHandle](const Created& signal)
                    {
                        return signal.handle == createdHandle;
                    }));
            }

            THEN("signal is emitted for known relic")
            {
                REQUIRE(knownCreatedSignals.Size() == 1);
                REQUIRE(knownCreatedSignals.begin()->reference == created);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(AsHandle(*created));

                THEN("generic signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.begin(),
                        genericDestroyingSignals.end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->reference == created);
                }
            }

            WHEN("clearing")
            {
                reliquary->Do<Clear>(Chroma::TypeIdentity<ClosedRelic>{});

                THEN("generic signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.begin(),
                        genericDestroyingSignals.end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->reference == created);
                }
            }

            WHEN("clearing by type")
            {
                reliquary->Do<Clear>(TypeFor<ClosedRelic>());

                THEN("generic signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.begin(),
                        genericDestroyingSignals.end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->reference == created);
                }
            }
        }

        WHEN("creating typed relic")
        {
            auto knownCreatedSignals = reliquary->Batch<CreatedKnown<TypedClosedRelic>>();
            auto knownDestroyingSignals = reliquary->Batch<DestroyingKnown<TypedClosedRelic>>();

            const auto created = reliquary->Do<Create<TypedClosedRelic>>();
            const auto createdHandle = AsHandle(*created);

            THEN("signal is emitted for relic and shard")
            {
                REQUIRE(genericCreatedSignals.Size() == 2);
                REQUIRE(std::any_of(
                    genericCreatedSignals.begin(),
                    genericCreatedSignals.end(),
                    [createdHandle](const Created& signal)
                    {
                        return signal.handle == createdHandle;
                    }));
            }

            THEN("signal is emitted for known relic")
            {
                REQUIRE(knownCreatedSignals.Size() == 1);
                REQUIRE(knownCreatedSignals.begin()->reference == created);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(AsHandle(*created));

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.begin(),
                        genericDestroyingSignals.end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->reference == created);
                }
            }

            WHEN("clearing")
            {
                reliquary->Do<Clear>(Chroma::TypeIdentity<TypedClosedRelic>{});

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.begin(),
                        genericDestroyingSignals.end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->reference == created);
                }
            }

            WHEN("clearing by type")
            {
                reliquary->Do<Clear>(TypeFor<TypedClosedRelic>());

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.begin(),
                        genericDestroyingSignals.end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Size() == 1);
                    REQUIRE(knownDestroyingSignals.begin()->reference == created);
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
            .Register<TypedOpenRelic>()
            .Register<Shard>()
            .Register<OtherShard>()
            .Actualize();

        auto relic = reliquary->Do<Create<TypedOpenRelic>>();

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
                REQUIRE(Arca::Index<OtherShard>(relic->ID(), *reliquary));
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
                    REQUIRE(!Arca::Index<OtherShard>(relic->ID(), *reliquary));
                }
            }

            WHEN("destroying shard via handle")
            {
                reliquary->Destroy(AsHandle<OtherShard>(relic->ID(), relic->Owner()));

                THEN("reliquary has destroyed shard")
                {
                    REQUIRE(reliquary->ShardSize() == 1);
                    REQUIRE(!Arca::Index<OtherShard>(relic->ID(), *reliquary));
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
            .Register<GlobalRelic>()
            .Register<Shard>()
            .Actualize();

        auto globalRelic = Arca::Index<GlobalRelic>(*reliquary);

        auto onParented = reliquary->Batch<RelicParented>();

        WHEN("parenting child to global parent")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do<CreateChild<OpenRelic>>(AsHandle(*globalRelic)),
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
            .Register<Shard>()
            .Actualize();

        auto parent = reliquary->Do<Create<OpenRelic>>();
        parent->Create<Shard>();

        auto onParented = reliquary->Batch<RelicParented>();

        WHEN("created child")
        {
            auto child = reliquary->Do<CreateChild<OpenRelic>>(AsHandle(*parent));
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
                REQUIRE(!Arca::Index<Shard>(child.ID(), *reliquary));
            }

            WHEN("destroying child")
            {
                reliquary->Destroy(AsHandle(*child));

                THEN("does not destroy parent")
                {
                    REQUIRE(reliquary->RelicSize() == 1);
                    REQUIRE(Arca::Index<Shard>(parent.ID(), *reliquary));
                }

                THEN("destroying parent works")
                {
                    reliquary->Destroy(AsHandle(*parent));

                    REQUIRE(reliquary->RelicSize() == 0);
                    REQUIRE(!Arca::Index<Shard>(parent.ID(), *reliquary));
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
                    reliquary->Do<CreateChild<OpenRelic>>(nonExistentRelic),
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
            .Register<Shard>();

        auto parentReliquary = origin.Actualize();
        auto childReliquary = origin.Actualize();

        auto parent = parentReliquary->Do<Create<OpenRelic>>();
        parent->Create<Shard>();

        WHEN("parenting child inside child reliquary")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    childReliquary->Do<CreateChild<OpenRelic>>(AsHandle(*parent)),
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
                    irrelevantReliquary->Do<CreateChild<OpenRelic>>(AsHandle(*parent)),
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
            .Register<InitializedRelic>()
            .Register<Shard>()
            .Actualize();

        WHEN("creating initialized relic")
        {
            auto myValue = dataGeneration.Random<int>();
            auto relic = reliquary->Do<Create<InitializedRelic>>(myValue);

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
            .Register<MovableOnlyRelic>()
            .Register<Shard>()
            .Actualize();

        WHEN("creating movable only relic")
        {
            auto myValue = dataGeneration.Random<int>();

            auto relic = reliquary->Do<Create<MovableOnlyRelic>>(myValue);

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

SCENARIO_METHOD(RelicTestsFixture, "relic constructed from moved value", "[relic]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<RelicConstructedFromMovedValue>()
            .Register<Shard>()
            .Actualize();

        WHEN("creating relic from moved value")
        {
            const auto generatedInt = dataGeneration.Random<int>();
            auto backingInt = new int(generatedInt);
            auto myInt = std::unique_ptr<int>(backingInt);

            auto relic = reliquary->Do<Create<RelicConstructedFromMovedValue>>(std::move(myInt));

            THEN("has moved value")
            {
                REQUIRE(relic->myInt.get() == backingInt);
            }
        }

        WHEN("creating child relic from moved value")
        {
            auto parent = reliquary->Do<Create<OpenRelic>>();

            const auto generatedInt = dataGeneration.Random<int>();
            auto backingInt = new int(generatedInt);
            auto myInt = std::unique_ptr<int>(backingInt);

            auto relic = reliquary->Do<CreateChild<RelicConstructedFromMovedValue>>(parent, std::move(myInt));

            THEN("has moved value")
            {
                REQUIRE(relic->myInt.get() == backingInt);
            }
        }

        WHEN("creating relic from moved value with structure")
        {
            auto structure = RelicStructure{};

            const auto generatedInt = dataGeneration.Random<int>();
            auto backingInt = new int(generatedInt);
            auto myInt = std::unique_ptr<int>(backingInt);

            auto relic = reliquary->Do<CreateWith<RelicConstructedFromMovedValue>>(structure, std::move(myInt));

            THEN("has moved value")
            {
                REQUIRE(relic->myInt.get() == backingInt);
            }
        }

        WHEN("creating child relic from moved value with structure")
        {
            auto parent = reliquary->Do<Create<OpenRelic>>();
            auto structure = RelicStructure{};

            const auto generatedInt = dataGeneration.Random<int>();
            auto backingInt = new int(generatedInt);
            auto myInt = std::unique_ptr<int>(backingInt);

            auto relic = reliquary->Do<CreateChildWith<RelicConstructedFromMovedValue>>(parent, structure, std::move(myInt));

            THEN("has moved value")
            {
                REQUIRE(relic->myInt.get() == backingInt);
            }
        }
    }
}