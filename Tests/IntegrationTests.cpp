#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "IntegrationTests.h"

#include "DifferentiableCurator.h"

IntegrationTestsFixture::ParentRelic::ParentRelic(Init init, int value)
    : ClosedTypedRelic(init), value(value)
{}

void IntegrationTestsFixture::ParentRelic::CreateChild() const
{
    Owner().Do(Arca::CreateChild<ChildRelic>{AsHandle(*this)});
}

IntegrationTestsFixture::MatrixCreatingRelic::MatrixCreatingRelic(Init init) :
    ClosedTypedRelic(init)
{
    init.Create<BasicShard>();
    init.Create<OtherShard>();
}

IntegrationTestsFixture::MatrixAndParentCurator::MatrixAndParentCurator(Init init) :
    Curator(init)
{
    Owner().On<MatrixFormed<All<BasicShard, OtherShard>>>(
        [this](const MatrixFormed<All<BasicShard, OtherShard>>& signal)
        {
            hadMatrixAndParent = static_cast<bool>(Owner().ParentOf(signal.index.ID()));
        });
}

IntegrationTestsFixture::RelicListeningToSignalFromConstructor::RelicListeningToSignalFromConstructor(Init init) :
    ClosedTypedRelic(init)
{
    Owner().On<BasicSignal>(
        [this](const BasicSignal& signal)
        {
            signalExecutions.push_back(signal.value);
        });
}

namespace Arca
{
    bool Traits<::IntegrationTestsFixture::ParentRelic>::ShouldCreate(Reliquary& reliquary, int value)
    {
        return value >= 100;
    }
}

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
            .CuratorPipeline(curatorPipeline)
            .Actualize();

        std::vector<DifferentiableCuratorBase*> curatorsInOrder
        {
            &reliquary->Find<DifferentiableCurator<1>>(),
            &reliquary->Find<DifferentiableCurator<4>>(),
            &reliquary->Find<DifferentiableCurator<3>>(),
            &reliquary->Find<DifferentiableCurator<2>>(),
            &reliquary->Find<DifferentiableCurator<0>>()
        };

        WHEN("raising signal then working reliquary")
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
                curator->onWork = [&encounteredSignals, &basicSignals](DifferentiableCuratorBase& self)
                {
                    for (auto& signal : basicSignals[self.Differentiator()])
                        encounteredSignals.emplace_back(&self, signal);
                };
            }

            const auto value = dataGeneration.Random<int>();
            reliquary->Raise(BasicSignal{ value });

            reliquary->Work();

            THEN("curators encounter signals in order")
            {
                REQUIRE(encounteredSignals.size() == 5);
                for(size_t i = 0; i < curatorsInOrder.size(); ++i)
                    REQUIRE(encounteredSignals[i].curator == curatorsInOrder[i]);
            }
        }
    }
}

SCENARIO_METHOD(
    IntegrationTestsFixture,
    "working with parent-child relics through curators",
    "[integration][relic][curator]")
{
    GIVEN("registered reliquary")
    {
        using ParentChildCurator = DifferentiableCurator<0>;

        auto reliquary = ReliquaryOrigin()
            .Register<ChildRelic>()
            .Register<ParentRelic>()
            .Register<ParentChildCurator>()
            .Actualize();

        std::unordered_map<int, Index<ParentRelic>> mappedParents;

        auto& curator = reliquary->Find<ParentChildCurator>();
        curator.onWork = [&mappedParents](DifferentiableCuratorBase& self)
        {
            auto value = 100;
            auto parent = self.TheOwner().Do(Create<ParentRelic>{value});
            parent->CreateChild();
            mappedParents.emplace(value, parent);
        };

        WHEN("working reliquary")
        {
            reliquary->Work();

            THEN("has created parent and child")
            {
                auto parents = reliquary->Batch<ParentRelic>();
                auto children = reliquary->Batch<ChildRelic>();

                REQUIRE(parents.begin() != parents.end());
                REQUIRE(children.begin() != children.end());

                auto parent = &*parents.begin();
                auto child = &*children.begin();

                REQUIRE(parent != nullptr);
                REQUIRE(child != nullptr);

                REQUIRE(!mappedParents.empty());
                REQUIRE(static_cast<const ParentRelic*>(mappedParents[100]) == parent);
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

        std::unordered_map<int, ParentRelic*> mappedParents;

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
    "curator relying on matrix formation and parenting sees both true",
    "[integration][matrix][signal][curator]")
{
    GIVEN("registered reliquary with curator, shard registered and shard created")
    {
        const auto reliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<OtherShard>()
            .Register<MatrixCreatingRelic>()
            .Register<MatrixAndParentCurator>()
            .Actualize();

        const auto parent = reliquary->Do(Create<OpenRelic>());

        WHEN("creating child relic")
        {
            reliquary->Do(CreateChild<MatrixCreatingRelic>(parent));

            THEN("curator has seen both matrix and parent")
            {
                auto& curator = reliquary->Find<MatrixAndParentCurator>();

                REQUIRE(curator.hadMatrixAndParent);
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
            .Register<BasicShard>()
            .Register<OtherShard>()
            .Register<DerivedCurator>()
            .Actualize();

        auto relic0 = savedReliquary->Do(Create<OpenRelic>());
        relic0->Create<BasicShard>();
        relic0->Create<OtherShard>();
        auto relic1 = savedReliquary->Do(Create<OpenRelic>());
        relic1->Create<BasicShard>();
        relic1->Create<OtherShard>();
        auto relic2 = savedReliquary->Do(Create<OpenRelic>());
        relic2->Create<BasicShard>();
        relic2->Create<OtherShard>();

        WHEN("saving reliquary")
        {
            {
                auto output = Inscription::OutputBinaryArchive("Test.dat");
                output(*savedReliquary);
            }

            WHEN("loading reliquary and taking batch in constructor")
            {
                DerivedCurator::onConstructor = [&allBatch](DifferentiableCuratorBase& curator)
                {
                    allBatch = curator.TheOwner().Batch<All<BasicShard, OtherShard>>();
                };

                const auto loadedReliquary = ReliquaryOrigin()
                    .Register<BasicShard>()
                    .Register<OtherShard>()
                    .Register<DerivedCurator>()
                    .Actualize();

                DerivedCurator::onConstructor = [&allBatch](DifferentiableCuratorBase& curator)
                {
                    allBatch = curator.TheOwner().Batch<All<BasicShard, OtherShard>>();
                };

                auto input = Inscription::InputBinaryArchive("Test.dat");
                input(*loadedReliquary);

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
    "reliquary continues to use previous ID sequence after serialization",
    "[integration][serialization]")
{
    GIVEN("registered reliquary with three open relics created")
    {
        const auto savedReliquary = ReliquaryOrigin()
            .Actualize();

        auto relic0 = savedReliquary->Do(Create<OpenRelic>());
        auto relic1 = savedReliquary->Do(Create<OpenRelic>());
        auto relic2 = savedReliquary->Do(Create<OpenRelic>());

        WHEN("saving reliquary")
        {
            {
                auto output = Inscription::OutputBinaryArchive("Test.dat");
                output(*savedReliquary);
            }

            WHEN("loading reliquary and creating three new open relics")
            {
                const auto loadedReliquary = ReliquaryOrigin()
                    .Actualize();

                auto input = Inscription::InputBinaryArchive("Test.dat");
                input(*loadedReliquary);

                auto relic3 = loadedReliquary->Do(Create<OpenRelic>());
                auto relic4 = loadedReliquary->Do(Create<OpenRelic>());
                auto relic5 = loadedReliquary->Do(Create<OpenRelic>());

                THEN("all loaded relics have greater IDs than saved")
                {
                    REQUIRE(relic3.ID() > relic0.ID());
                    REQUIRE(relic3.ID() > relic1.ID());
                    REQUIRE(relic3.ID() > relic2.ID());
                    REQUIRE(relic4.ID() > relic0.ID());
                    REQUIRE(relic4.ID() > relic1.ID());
                    REQUIRE(relic4.ID() > relic2.ID());
                    REQUIRE(relic5.ID() > relic0.ID());
                    REQUIRE(relic5.ID() > relic1.ID());
                    REQUIRE(relic5.ID() > relic2.ID());
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
            .Register<BasicShard>()
            .Register<OtherShard>()
            .Register<DerivedCurator>()
            .Actualize();

        auto& curator = reliquary->Find<DerivedCurator>();

        auto relic = reliquary->Do(Create<OpenRelic>());
        auto shard = relic->Create<BasicShard>();

        auto setValue = dataGeneration.Random<int>();

        curator.onWork = [relic, setValue](DifferentiableCuratorBase& self)
        {
            auto data = self.TheData<BasicShard>(relic.ID());
            data->value = setValue;
        };

        reliquary->Work();

        WHEN("querying shard")
        {
            auto afterShard = relic->Find<BasicShard>();

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