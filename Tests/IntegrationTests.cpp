#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "IntegrationTests.h"

#include "DifferentiableCurator.h"
#include <Arca/OpenRelic.h>
#include <Arca/All.h>
#include <Arca/Create.h>

#include <Inscription/Binary.h>

IntegrationTestsFixture::MatrixCreatingRelic::MatrixCreatingRelic(RelicInit init)
{
    init.Create<BasicShard>();
    init.Create<OtherShard>();
}

IntegrationTestsFixture::RelicListeningToSignalFromConstructor::RelicListeningToSignalFromConstructor(
    RelicInit init)
{
    init.owner.On<BasicSignal>(
        [this](const BasicSignal& signal)
        {
            signalExecutions.push_back(signal.value);
        });
}

IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic::GlobalRelicCreatingNullSerializedRelic(
    RelicInit init)
    :
    localRelic(init.owner.Do(Arca::Create<LocalRelic>()))
{}

IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic::GlobalRelicCreatingNullSerializedRelic(
    RelicInit init, Serialization)
{}

template<size_t differentiator>
void SetupDifferentiableCurator(std::unordered_map<size_t, std::list<IntegrationTestsFixture::BasicSignal>>& basicSignals)
{
    using BasicSignal = IntegrationTestsFixture::BasicSignal;

    DifferentiableCurator<differentiator>::onConstructor = [&basicSignals](DifferentiableCurator<differentiator>& curator)
    {
        auto& list = basicSignals.emplace(differentiator, std::list<BasicSignal>{})
            .first->second;

        curator.TheOwner().template On<BasicSignal>([&list](const BasicSignal& signal)
            {
                list.push_back(signal);
            });
    };
}

SCENARIO_METHOD(IntegrationTestsFixture, "working with signals through curators", "[integration][curator][signal]")
{
    GIVEN("registered reliquary")
    {
        const auto curatorPipeline = Pipeline
        {
            Stage::All<DifferentiableCurator<1>>(),
            Stage::All<DifferentiableCurator<4>, DifferentiableCurator<3>>(),
            Stage::All<DifferentiableCurator<2>, DifferentiableCurator<0>>()
        };

        std::unordered_map<size_t, std::list<BasicSignal>> basicSignals;

        SetupDifferentiableCurator<0>(basicSignals);
        SetupDifferentiableCurator<1>(basicSignals);
        SetupDifferentiableCurator<2>(basicSignals);
        SetupDifferentiableCurator<3>(basicSignals);
        SetupDifferentiableCurator<4>(basicSignals);

        auto reliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<DifferentiableCurator<0>>()
            .Register<DifferentiableCurator<1>>()
            .Register<DifferentiableCurator<2>>()
            .Register<DifferentiableCurator<3>>()
            .Register<DifferentiableCurator<4>>()
            .CuratorConstructionPipeline(curatorPipeline)
            .Actualize();

        std::vector<DifferentiableCuratorBase*> curatorsInOrder
        {
            &reliquary->Find<DifferentiableCurator<1>>(),
            &reliquary->Find<DifferentiableCurator<4>>(),
            &reliquary->Find<DifferentiableCurator<3>>(),
            &reliquary->Find<DifferentiableCurator<2>>(),
            &reliquary->Find<DifferentiableCurator<0>>()
        };

        WHEN("raising signal then sending command")
        {
            struct EncounteredSignal
            {
                DifferentiableCuratorBase* curator;
                BasicSignal signal;

                EncounteredSignal(DifferentiableCuratorBase* curator, BasicSignal signal) :
                    curator(curator), signal(signal)
                {}
            };
            std::vector<EncounteredSignal> encounteredSignals;

            for(auto& curator : curatorsInOrder)
            {
                curator->onCommand = [&encounteredSignals, &basicSignals](DifferentiableCuratorBase& self)
                {
                    for (auto& signal : basicSignals[self.Differentiator()])
                        encounteredSignals.emplace_back(&self, signal);
                };
            }

            const auto value = dataGeneration.Random<int>();
            reliquary->Raise(BasicSignal{ value });

            reliquary->Do(BasicCommand{});

            THEN("curators encounter signals in order")
            {
                REQUIRE(encounteredSignals.size() == 5);
                for(size_t i = 0; i < curatorsInOrder.size(); ++i)
                    REQUIRE(encounteredSignals[i].curator == curatorsInOrder[i]);
            }
        }
    }
}

SCENARIO_METHOD(IntegrationTestsFixture, "curators with custom signal execution", "[integration][curator][signal]")
{
    GIVEN("registered reliquary")
    {
        std::vector<BasicSignal> executedSignals;

        DerivedCurator::onConstructor = [&executedSignals](DifferentiableCuratorBase& curator)
        {
            curator.TheOwner().On<BasicSignal>([&executedSignals](const BasicSignal& signal)
                {
                    executedSignals.push_back(signal);
                });
        };

        auto reliquary = ReliquaryOrigin()
            .Register<DerivedCurator>()
            .Actualize();

        WHEN("raising signal many times")
        {
            for (auto loop = 0; loop < 1000; ++loop)
                reliquary->Raise(BasicSignal{ loop });

            THEN("executed signals contains all signals raised")
            {
                REQUIRE(executedSignals.size() == 1000);
                int i = 0;
                REQUIRE(std::all_of(
                    executedSignals.begin(),
                    executedSignals.end(),
                    [&i](const BasicSignal& signal)
                    {
                        const auto returnValue = signal.value == i;
                        ++i;
                        return returnValue;
                    }));
            }
        }
    }
}

SCENARIO_METHOD(
    IntegrationTestsFixture,
    "all batch with either in it is occupied after loading",
    "[integration][curator][all][either][serialization]")
{
    GIVEN("registered reliquary and 3 relics with both shards")
    {
        Batch<All<BasicShard, OtherShard>> allBatch;

        DerivedCurator::onConstructor = [&allBatch](DifferentiableCuratorBase& curator)
        {
            allBatch = curator.TheOwner().Batch<All<BasicShard, OtherShard>>();
        };

        const auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Register<OtherShard>()
            .Register<DerivedCurator>()
            .Actualize();

        const auto relic0 = savedReliquary->Do(Create<OpenRelic>());
        savedReliquary->Do(Create<BasicShard>(relic0));
        savedReliquary->Do(Create<OtherShard>(relic0));
        const auto relic1 = savedReliquary->Do(Create<OpenRelic>());
        savedReliquary->Do(Create<BasicShard>(relic1));
        savedReliquary->Do(Create<OtherShard>(relic1));
        const auto relic2 = savedReliquary->Do(Create<OpenRelic>());
        savedReliquary->Do(Create<BasicShard>(relic2));
        savedReliquary->Do(Create<OtherShard>(relic2));

        WHEN("saving reliquary")
        {
            Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

            WHEN("loading reliquary and taking batch in constructor")
            {
                DerivedCurator::onConstructor = [&allBatch](DifferentiableCuratorBase& curator)
                {
                    allBatch = curator.TheOwner().Batch<All<BasicShard, OtherShard>>();
                };

                const auto loadedReliquary = ReliquaryOrigin()
                    .Register<OpenRelic>()
                    .Register<BasicShard>()
                    .Register<OtherShard>()
                    .Register<DerivedCurator>()
                    .Actualize();

                Inscription::Binary::FromFile(*loadedReliquary, "Test.dat");

                THEN("batch is occupied")
                {
                    REQUIRE(allBatch.Size() == 3);
                }
            }
        }
    }
}

SCENARIO_METHOD(
    IntegrationTestsFixture,
    "curator can mutate data",
    "[integration][serialization]")
{
    GIVEN("registered reliquary with curator, shard registered and shard created")
    {
        const auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Register<OtherShard>()
            .Register<DerivedCurator>()
            .Actualize();

        auto& curator = reliquary->Find<DerivedCurator>();

        auto relic = reliquary->Do(Create<OpenRelic>());
        auto shard = reliquary->Do(Create<BasicShard>(relic));

        auto setValue = dataGeneration.Random<int>();

        curator.onCommand = [relic, setValue](DifferentiableCuratorBase& self)
        {
            const auto data = self.TheData<BasicShard>(relic.ID());
            data->value = setValue;
        };

        reliquary->Do(BasicCommand{});

        WHEN("querying shard")
        {
            auto afterShard = reliquary->Find<BasicShard>(relic.ID());

            THEN("data has been set")
            {
                REQUIRE(afterShard->value == setValue);
            }
        }

        WHEN("querying index set before modification")
        {
            THEN("data has been set")
            {
                REQUIRE(shard->value == setValue);
            }
        }
    }
}

SCENARIO_METHOD(
    IntegrationTestsFixture,
    "relic can listen to signal from constructor",
    "[integration][relic][signal]")
{
    GIVEN("registered reliquary with relic")
    {
        const auto reliquary = ReliquaryOrigin()
            .Register<RelicListeningToSignalFromConstructor>()
            .Actualize();

        auto relic = reliquary->Do(Create<RelicListeningToSignalFromConstructor>{});

        WHEN("emitting signal")
        {
            const auto value = dataGeneration.Random<int>();

            reliquary->Raise(BasicSignal{ value });

            THEN("relic has captured execution")
            {
                REQUIRE(relic->signalExecutions.size() == 1);
                REQUIRE(relic->signalExecutions[0] == value);
            }
        }
    }
}

SCENARIO_METHOD(
    IntegrationTestsFixture,
    "global relic's local relic that is null serialized is constructed on load",
    "[integration][relic][serialization]")
{
    GIVEN("registered reliquary with relic")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Register<GlobalRelicCreatingNullSerializedRelic>()
            .Register<GlobalRelicCreatingNullSerializedRelic::LocalShard>()
            .Register<GlobalRelicCreatingNullSerializedRelic::LocalRelic>()
            .Actualize();

        WHEN("saving and loading")
        {
            Inscription::Binary::ToFile(*savedReliquary, "Test.dat");

            const auto loadedReliquary = ReliquaryOrigin()
                .Register<GlobalRelicCreatingNullSerializedRelic>()
                .Register<GlobalRelicCreatingNullSerializedRelic::LocalShard>()
                .Register<GlobalRelicCreatingNullSerializedRelic::LocalRelic>()
                .Actualize();

            Inscription::Binary::ToFile(*loadedReliquary, "Test.dat");

            auto globalRelic = loadedReliquary->Find<GlobalRelicCreatingNullSerializedRelic>();

            THEN("local relic is setup correctly")
            {
                REQUIRE(globalRelic->localRelic);
            }
        }
    }
}

SCENARIO_METHOD(IntegrationTestsFixture, "listening for both non-const and const shard created", "[integration][shard][signal]")
{
    GIVEN("registered reliquary with shard")
    {
        const auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        WHEN("listening for both non-const and const shard created and creating both")
        {
            auto nonConstSignaled = 0;
            auto constSignaled = 0;

            reliquary->On<CreatedKnown<BasicShard>>(
                [&nonConstSignaled](const CreatedKnown<BasicShard>&)
                {
                    ++nonConstSignaled;
                });

            reliquary->On<CreatedKnown<const BasicShard>>(
                [&constSignaled](const CreatedKnown<const BasicShard>&)
                {
                    ++constSignaled;
                });

            const auto nonConstRelic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<BasicShard>(nonConstRelic.ID()));

            const auto constRelic = reliquary->Do(Create<OpenRelic>());
            reliquary->Do(Create<const BasicShard>(constRelic.ID()));

            THEN("has signaled both once")
            {
                REQUIRE(nonConstSignaled == 1);
                REQUIRE(constSignaled == 1);
            }
        }
    }
}