#include <catch.hpp>
using namespace std::string_literals;

#include "RelicTests.h"
#include "SignalListener.h"

#include <Arca/LocalRelic.h>
#include <Arca/RelicParented.h>

RelicTestsFixture::TypedClosedRelic::TypedClosedRelic(RelicInit init)
{
    basicShard = init.Create<Shard>();
}

RelicTestsFixture::TypedOpenRelic::TypedOpenRelic(RelicInit init)
{
    basicShard = init.Create<Shard>();
}

RelicTestsFixture::GlobalRelic::GlobalRelic(RelicInit init)
{
    basicShard = init.Create<Shard>();
}

RelicTestsFixture::ShouldCreateRelic::ShouldCreateRelic(RelicInit init)
{}

RelicTestsFixture::ShouldCreateRelic::ShouldCreateRelic(RelicInit init, int value)
    : value(value)
{}

RelicTestsFixture::InitializedRelic::InitializedRelic(RelicInit init)
{
    basicShard = init.Create<Shard>();
}

RelicTestsFixture::InitializedRelic::InitializedRelic(RelicInit init, int value)
    : myValue(value)
{
    basicShard = init.Create<Shard>();
}

RelicTestsFixture::MovableOnlyRelic::MovableOnlyRelic(RelicInit init)
{
    basicShard = init.Create<Shard>();
}

RelicTestsFixture::MovableOnlyRelic::MovableOnlyRelic(RelicInit init, int myInt)
    : myValue(myInt)
{
    basicShard = init.Create<Shard>();
}

RelicTestsFixture::RelicConstructedFromMovedValue::RelicConstructedFromMovedValue(RelicInit init)
{}

RelicTestsFixture::RelicConstructedFromMovedValue::RelicConstructedFromMovedValue(
    RelicInit init, std::unique_ptr<int>&& myInt)
    :
    myInt(std::move(myInt))
{}

RelicTestsFixture::RelicWithShouldCreateAndMovedValue::RelicWithShouldCreateAndMovedValue(RelicInit init)
{}

RelicTestsFixture::RelicWithShouldCreateAndMovedValue::RelicWithShouldCreateAndMovedValue(RelicInit init, std::unique_ptr<int>&& myInt) :
    myInt(std::move(myInt))
{}

namespace Arca
{
    bool Traits<RelicTestsFixture::ShouldCreateRelic>::ShouldCreate(Reliquary& reliquary, int value)
    {
        return value >= 100;
    }

    bool Traits<RelicTestsFixture::RelicWithShouldCreateAndMovedValue>::ShouldCreate(Reliquary& reliquary, std::unique_ptr<int>& myInt)
    {
        return *myInt >= 100;
    }
}

SCENARIO_METHOD(RelicTestsFixture, "relic", "[relic]")
{
    GIVEN("default constructed relic registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<DefaultConstructedRelic>()
            .Actualize();

        WHEN("creating")
        {
            auto created = reliquary->Do(Arca::Create<DefaultConstructedRelic>());

            THEN("is created")
            {
                REQUIRE(created);
            }
        }
    }

    GIVEN("all types registered")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<ClosedRelic>()
            .Register<Shard>()
            .Register<OtherShard>()
            .Register<TypedClosedRelic>()
            .Register<GlobalRelic>()
            .Actualize();

        WHEN("creating open relic")
        {
            auto preCreateRelicSize = reliquary->RelicSize();
            auto openRelic = reliquary->Do(Create<OpenRelic>());

            THEN("does not have parent")
            {
                REQUIRE(!reliquary->ParentOf(openRelic.ID()));
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
                auto found = Arca::Index<OpenRelic>(openRelic.ID(), *reliquary);

                THEN("found is same as created")
                {
                    REQUIRE(found.ID() == openRelic.ID());
                    REQUIRE(found.Owner() == openRelic.Owner());
                }
            }

            WHEN("creating shard")
            {
                auto preCreateShardSize = reliquary->ShardSize();
                auto shard = reliquary->Do(Arca::Create<Shard>(openRelic.ID()));

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
                    REQUIRE(Arca::Index<Shard>(openRelic.ID(), *reliquary));
                    REQUIRE(Arca::Index<Either<Shard>>(openRelic.ID(), *reliquary));
                    REQUIRE(reliquary->Contains<Shard>(openRelic.ID()));
                }

                WHEN("destroying shard")
                {
                    reliquary->Do(Arca::Destroy<Shard>(openRelic.ID()));

                    THEN("reliquary loses a shard")
                    {
                        REQUIRE(reliquary->ShardSize() == preCreateRelicSize);
                    }

                    THEN("reliquary does not have shard")
                    {
                        REQUIRE(!Arca::Index<Shard>(openRelic.ID(), *reliquary));
                        REQUIRE(!Arca::Index<Either<Shard>>(openRelic.ID(), *reliquary));
                        REQUIRE(!reliquary->Contains<Shard>(openRelic.ID()));
                    }
                }
            }

            WHEN("retrieving as closed")
            {
                auto closedRelic = Arca::Index<ClosedRelic>(openRelic.ID(), *reliquary);

                THEN("closed relic is not found")
                {
                    REQUIRE(!closedRelic);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::Index<TypedClosedRelic>(openRelic.ID(), *reliquary);

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }

        WHEN("creating closed relic with valid structure")
        {
            auto preCreateRelicCount = reliquary->RelicSize();
            auto closedRelic = reliquary->Do(CreateWith<ClosedRelic>{RelicStructure{ TypeFor<Shard>() }});

            THEN("structure has been satisfied")
            {
                REQUIRE(Arca::Index<Shard>(closedRelic.ID(), *reliquary));
                REQUIRE(reliquary->Contains<Shard>(closedRelic.ID()));
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
                auto found = Arca::Index<ClosedRelic>(closedRelic.ID(), *reliquary);

                THEN("found is same as created")
                {
                    REQUIRE(found.ID() == closedRelic.ID());
                    REQUIRE(found.Owner() == closedRelic.Owner());
                }
            }

            WHEN("destroying relic")
            {
                auto preDestroyRelicCount = reliquary->RelicSize();

                auto id = closedRelic.ID();
                auto handle = AsHandle(closedRelic);
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
                const auto asOpen = Arca::Index<OpenRelic>(closedRelic.ID(), *reliquary);

                THEN("open is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::Index<TypedClosedRelic>(closedRelic.ID(), *reliquary);

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }

        WHEN("creating typed relic")
        {
            auto preCreateRelicSize = reliquary->RelicSize();
            auto typedRelic = reliquary->Do(Create<TypedClosedRelic>());

            THEN("does not have parent")
            {
                REQUIRE(!reliquary->ParentOf(typedRelic.ID()));
            }

            THEN("reliquary has one more relic")
            {
                REQUIRE(reliquary->RelicSize() == (preCreateRelicSize + 1));
            }

            THEN("reliquary contains relic")
            {
                REQUIRE(reliquary->Contains<TypedClosedRelic>(typedRelic.ID()));
            }

            WHEN("finding typed relic")
            {
                auto found = Arca::Index<TypedClosedRelic>(typedRelic.ID(), *reliquary);

                THEN("found is same as created")
                {
                    REQUIRE(found.ID() == typedRelic.ID());
                    REQUIRE(found.Owner() == typedRelic.Owner());
                }
            }

            THEN("has shard")
            {
                REQUIRE(typedRelic->basicShard);
            }

            WHEN("retrieving as open")
            {
                const auto asOpen = Arca::Index<OpenRelic>(typedRelic.ID(), *reliquary);

                THEN("typed is empty")
                {
                    REQUIRE(!asOpen);
                }
            }

            WHEN("retrieving as closed")
            {
                auto closedRelic = Arca::Index<ClosedRelic>(typedRelic.ID(), *reliquary);

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
            .Register<OpenRelic>()
            .Register<Shard>()
            .Register<OtherShard>()
            .Actualize();

        std::vector<Index<OpenRelic>> relics;
        for (size_t i = 0; i < 100; ++i)
        {
            relics.push_back(reliquary->Do(Create<OpenRelic>()));
            reliquary->Do(Arca::Create<Shard>(static_cast<int>(i + 1), static_cast<int>(i)));
        }

        WHEN("deleting all but the last")
        {
            auto loop = relics.begin();
            while (loop != --relics.end())
            {
                reliquary->Destroy(AsHandle(*loop));
                loop = relics.erase(loop);
            }

            THEN("last relic still valid")
            {
                auto relic = relics[0];

                auto shard = Arca::Index<Shard>(relic.ID(), *reliquary);
                REQUIRE(shard);
                REQUIRE(shard->value == 99);
            }
        }

        WHEN("clearing")
        {
            reliquary->Do(Clear{ Chroma::TypeIdentity<OpenRelic>{} });

            THEN("reliquary is empty")
            {
                REQUIRE(reliquary->RelicSize() == 0);
                REQUIRE(reliquary->ShardSize() == 0);
            }
        }

        WHEN("clearing by type")
        {
            reliquary->Do(Clear{ TypeFor<OpenRelic>() });

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
            const auto relic = reliquary->Do(Create<ShouldCreateRelic>{100});

            THEN("relic was created")
            {
                REQUIRE(relic);
                REQUIRE(Arca::Index<ShouldCreateRelic>(relic.ID(), *reliquary));
            }
        }

        WHEN("creating relic with 99 value")
        {
            const auto relic = reliquary->Do(Create<ShouldCreateRelic>{99});

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
            .Register<OpenRelic>()
            .Register<ClosedRelic>()
            .Register<TypedClosedRelic>()
            .Register<Shard>()
            .Actualize();

        auto genericCreatedSignals = SignalListener<Created>(*reliquary);
        auto genericDestroyingSignals = SignalListener<Destroying>(*reliquary);

        WHEN("creating open relic")
        {
            auto knownCreatedSignals = SignalListener<CreatedKnown<OpenRelic>>(*reliquary);
            auto knownDestroyingSignals = SignalListener<DestroyingKnown<OpenRelic>>(*reliquary);

            const auto created = reliquary->Do(Create<OpenRelic>());
            const auto createdHandle = AsHandle(created);

            THEN("signal is emitted for relic")
            {
                REQUIRE(genericCreatedSignals.Executions().size() == 1);
                REQUIRE(genericCreatedSignals.Executions().begin()->handle == createdHandle);
                REQUIRE(knownCreatedSignals.Executions().size() == 1);
                REQUIRE(knownCreatedSignals.Executions().begin()->index == created);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(AsHandle(created));

                THEN("signal is emitted for relic")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == createdHandle);
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == created);
                }
            }

            WHEN("clearing")
            {
                reliquary->Do(Clear{ Chroma::TypeIdentity<OpenRelic>{} });

                THEN("signal is emitted for relic")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == createdHandle);
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == created);
                }
            }

            WHEN("clearing by type")
            {
                reliquary->Do(Clear{ TypeFor<OpenRelic>() });

                THEN("signal is emitted for relic")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 1);
                    REQUIRE(genericDestroyingSignals.Executions().begin()->handle == createdHandle);
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == created);
                }
            }
        }

        WHEN("creating closed relic")
        {
            auto knownCreatedSignals = SignalListener<CreatedKnown<ClosedRelic>>(*reliquary);
            auto knownDestroyingSignals = SignalListener<DestroyingKnown<ClosedRelic>>(*reliquary);

            const auto created = reliquary->Do(CreateWith<ClosedRelic>{RelicStructure{ TypeFor<Shard>() }});
            const auto createdHandle = AsHandle(created);

            THEN("generic signal is emitted for relic and shard")
            {
                REQUIRE(genericCreatedSignals.Executions().size() == 2);
                REQUIRE(std::any_of(
                    genericCreatedSignals.Executions().begin(),
                    genericCreatedSignals.Executions().end(),
                    [createdHandle](const Created& signal)
                    {
                        return signal.handle == createdHandle;
                    }));
            }

            THEN("signal is emitted for known relic")
            {
                REQUIRE(knownCreatedSignals.Executions().size() == 1);
                REQUIRE(knownCreatedSignals.Executions().begin()->index == created);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(AsHandle(created));

                THEN("generic signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.Executions().begin(),
                        genericDestroyingSignals.Executions().end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == created);
                }
            }

            WHEN("clearing")
            {
                reliquary->Do(Clear{ Chroma::TypeIdentity<ClosedRelic>{} });

                THEN("generic signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.Executions().begin(),
                        genericDestroyingSignals.Executions().end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == created);
                }
            }

            WHEN("clearing by type")
            {
                reliquary->Do(Clear{ TypeFor<ClosedRelic>() });

                THEN("generic signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.Executions().begin(),
                        genericDestroyingSignals.Executions().end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == created);
                }
            }
        }

        WHEN("creating typed relic")
        {
            auto knownCreatedSignals = SignalListener<CreatedKnown<TypedClosedRelic>>(*reliquary);
            auto knownDestroyingSignals = SignalListener<DestroyingKnown<TypedClosedRelic>>(*reliquary);

            const auto created = reliquary->Do(Create<TypedClosedRelic>());
            const auto createdHandle = AsHandle(created);

            THEN("signal is emitted for relic and shard")
            {
                REQUIRE(genericCreatedSignals.Executions().size() == 2);
                REQUIRE(std::any_of(
                    genericCreatedSignals.Executions().begin(),
                    genericCreatedSignals.Executions().end(),
                    [createdHandle](const Created& signal)
                    {
                        return signal.handle == createdHandle;
                    }));
            }

            THEN("signal is emitted for known relic")
            {
                REQUIRE(knownCreatedSignals.Executions().size() == 1);
                REQUIRE(knownCreatedSignals.Executions().begin()->index == created);
            }

            WHEN("destroying relic")
            {
                reliquary->Destroy(AsHandle(created));

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.Executions().begin(),
                        genericDestroyingSignals.Executions().end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == created);
                }
            }

            WHEN("clearing")
            {
                reliquary->Do(Clear{ Chroma::TypeIdentity<TypedClosedRelic>{} });

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.Executions().begin(),
                        genericDestroyingSignals.Executions().end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == created);
                }
            }

            WHEN("clearing by type")
            {
                reliquary->Do(Clear{ TypeFor<TypedClosedRelic>() });

                THEN("signal is emitted for relic and shard")
                {
                    REQUIRE(genericDestroyingSignals.Executions().size() == 2);
                    REQUIRE(std::any_of(
                        genericDestroyingSignals.Executions().begin(),
                        genericDestroyingSignals.Executions().end(),
                        [createdHandle](const Destroying& signal)
                        {
                            return signal.handle == createdHandle;
                        }));
                }

                THEN("signal is emitted for known relic")
                {
                    REQUIRE(knownDestroyingSignals.Executions().size() == 1);
                    REQUIRE(knownDestroyingSignals.Executions().begin()->index == created);
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

        auto relic = reliquary->Do(Create<TypedOpenRelic>{});

        WHEN("default created")
        {
            THEN("has all requested shards")
            {
                REQUIRE(relic->basicShard);
            }
        }

        WHEN("creating dynamic shard")
        {
            auto shard = reliquary->Do(Arca::Create<OtherShard>(relic.ID()));

            THEN("returned is valid")
            {
                REQUIRE(shard);
            }

            THEN("reliquary created shard")
            {
                REQUIRE(reliquary->ShardSize() == 2);
                REQUIRE(Arca::Index<OtherShard>(relic.ID(), *reliquary));
            }

            WHEN("finding by either")
            {
                auto found = Arca::Index<Either<OtherShard>>(relic.ID(), *reliquary);

                THEN("is same as created")
                {
                    REQUIRE(found == shard);
                }
            }

            WHEN("destroying dynamic shard")
            {
                reliquary->Do(Arca::Destroy<OtherShard>(relic.ID()));

                THEN("reliquary has destroyed shard")
                {
                    REQUIRE(reliquary->ShardSize() == 1);
                    REQUIRE(!Arca::Index<OtherShard>(relic.ID(), *reliquary));
                }
            }

            WHEN("destroying shard via handle")
            {
                reliquary->Destroy(AsHandle<OtherShard>(relic.ID(), *relic.Owner()));

                THEN("reliquary has destroyed shard")
                {
                    REQUIRE(reliquary->ShardSize() == 1);
                    REQUIRE(!Arca::Index<OtherShard>(relic.ID(), *reliquary));
                }
            }

            WHEN("creating same shard type with const")
            {
                THEN("throws error")
                {
                    REQUIRE_THROWS_AS(reliquary->Do(Arca::Create<const OtherShard>(relic.ID())), CannotCreate);
                }
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
            auto relic = reliquary->Do(Create<InitializedRelic>{myValue});

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

            auto relic = reliquary->Do(Create<MovableOnlyRelic>{myValue});

            THEN("has value")
            {
                REQUIRE(relic->myValue == myValue);
            }

            THEN("has shard")
            {
                REQUIRE(relic->basicShard);
            }

            THEN("batch contains relic")
            {
                auto batch = reliquary->Batch<MovableOnlyRelic>();
                REQUIRE(batch.Size() == 1);
                REQUIRE(batch.begin()->myValue == myValue);
            }

            THEN("can extract values from loop")
            {
                auto batch = reliquary->Batch<MovableOnlyRelic>();

                std::vector<int> values;
                for (auto& relic : batch)
                    values.push_back(relic.myValue);

                REQUIRE(values.size() == 1);
                REQUIRE(values[0] == myValue);
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "relic constructed from moved value", "[relic]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<RelicConstructedFromMovedValue>()
            .Register<Shard>()
            .Actualize();

        WHEN("creating relic from moved value")
        {
            const auto value = dataGeneration.Random<int>();
            auto uniquePtr = std::make_unique<int>(value);

            auto relic = reliquary->Do(Create<RelicConstructedFromMovedValue>{std::move(uniquePtr)});

            THEN("has moved value")
            {
                REQUIRE(*relic->myInt == value);
            }
        }

        WHEN("creating child relic from moved value")
        {
            auto parent = reliquary->Do(Create<OpenRelic>());

            const auto value = dataGeneration.Random<int>();
            auto uniquePtr = std::make_unique<int>(value);

            auto relic = reliquary->Do(CreateChild<RelicConstructedFromMovedValue>{parent, std::move(uniquePtr)});

            THEN("has moved value")
            {
                REQUIRE(*relic->myInt == value);
            }
        }

        WHEN("creating relic from moved value with structure")
        {
            auto structure = RelicStructure{};

            const auto value = dataGeneration.Random<int>();
            auto uniquePtr = std::make_unique<int>(value);

            auto relic = reliquary->Do(CreateWith<RelicConstructedFromMovedValue>{structure, std::move(uniquePtr)});

            THEN("has moved value")
            {
                REQUIRE(*relic->myInt == value);
            }
        }

        WHEN("creating child relic from moved value with structure")
        {
            auto parent = reliquary->Do(Create<OpenRelic>());
            auto structure = RelicStructure{};

            const auto value = dataGeneration.Random<int>();
            auto uniquePtr = std::make_unique<int>(value);

            auto relic = reliquary->Do(CreateChildWith<RelicConstructedFromMovedValue>{parent, structure, std::move(uniquePtr)});

            THEN("has moved value")
            {
                REQUIRE(*relic->myInt == value);
            }
        }
    }
}

SCENARIO_METHOD(RelicTestsFixture, "relic moving only and shouldCreate", "[relic]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<RelicWithShouldCreateAndMovedValue>()
            .Actualize();

        WHEN("creating relic from unordered_set")
        {
            const auto intValue = dataGeneration.Random<int>();
            auto myInt = std::make_unique<int>(intValue);

            auto relic = reliquary->Do(Create<RelicWithShouldCreateAndMovedValue>{std::move(myInt)});

            THEN("has ints")
            {
                REQUIRE(*relic->myInt == intValue);
            }
        }
    }
}