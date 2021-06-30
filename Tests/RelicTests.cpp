#include <catch.hpp>
using namespace std::string_literals;

#include "RelicTests.h"
#include "SignalListener.h"

#include <Arca/OpenRelic.h>
#include <Arca/RelicParented.h>

RelicTestsFixture::LocalRelic::LocalRelic(RelicInit init)
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
            .Register<Shard>()
            .Register<OtherShard>()
            .Register<LocalRelic>()
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

            THEN("relic type has size one")
            {
                REQUIRE(reliquary->RelicSize(TypeFor<OpenRelic>().name) == 1);
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

                THEN("shard type has size one")
                {
                    REQUIRE(reliquary->ShardSize(TypeFor<Shard>()) == preCreateShardSize + 1);
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

            WHEN("retrieving as local")
            {
                auto localRelic = Arca::Index<LocalRelic>(openRelic.ID(), *reliquary);

                THEN("is not found")
                {
                    REQUIRE(!localRelic);
                }
            }

            WHEN("retrieving as typed")
            {
                const auto asTyped = Arca::Index<LocalRelic>(openRelic.ID(), *reliquary);

                THEN("typed is empty")
                {
                    REQUIRE(!asTyped);
                }
            }
        }
        
        WHEN("creating typed relic")
        {
            auto preCreateRelicSize = reliquary->RelicSize();
            auto typedRelic = reliquary->Do(Create<LocalRelic>());

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
                REQUIRE(reliquary->Contains<LocalRelic>(typedRelic.ID()));
            }

            WHEN("finding typed relic")
            {
                auto found = Arca::Index<LocalRelic>(typedRelic.ID(), *reliquary);

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
            .Register<LocalRelic>()
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
        
        WHEN("creating typed relic")
        {
            auto knownCreatedSignals = SignalListener<CreatedKnown<LocalRelic>>(*reliquary);
            auto knownDestroyingSignals = SignalListener<DestroyingKnown<LocalRelic>>(*reliquary);

            const auto created = reliquary->Do(Create<LocalRelic>());
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
                reliquary->Do(Clear{ Chroma::TypeIdentity<LocalRelic>{} });

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
                reliquary->Do(Clear{ TypeFor<LocalRelic>() });

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
            const auto intValue = 101;
            auto myInt = std::make_unique<int>(intValue);

            auto relic = reliquary->Do(Create<RelicWithShouldCreateAndMovedValue>{std::move(myInt)});

            THEN("has ints")
            {
                REQUIRE(*relic->myInt == intValue);
            }
        }
    }
}