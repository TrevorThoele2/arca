#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "IntegrationTests.h"

#include "DifferentiableCurator.h"

IntegrationTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

IntegrationTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

IntegrationTestsFixture::ParentRelic::ParentRelic(Init init, int value)
    : ClosedTypedRelic(init), value(value)
{}

void IntegrationTestsFixture::ParentRelic::CreateChild() const
{
    Owner().Do<Arca::CreateChild<ChildRelic>>(AsHandle(*this));
}

namespace Arca
{
    bool Traits<::IntegrationTestsFixture::ParentRelic>::ShouldCreate(Reliquary& reliquary, int value)
    {
        return value >= 100;
    }
}

template<size_t differentiator>
void SetupDifferentiableCurator(std::unordered_map<size_t, Batch<IntegrationTestsFixture::BasicSignal>>& basicSignals)
{
    DifferentiableCurator<differentiator>::onConstructor = [&basicSignals](DifferentiableCurator<differentiator>& curator)
    {
        basicSignals.emplace(differentiator, curator.TheOwner()
            .template Batch<IntegrationTestsFixture::BasicSignal>());
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

        std::unordered_map<size_t, Batch<BasicSignal>> basicSignals;

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

            THEN("signals are cleared")
            {
                const auto signalBatch = reliquary->Batch<BasicSignal>();
                REQUIRE(signalBatch.IsEmpty());
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
            auto parent = self.TheOwner().Do<Create<ParentRelic>>(value);
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
            curator.TheOwner().ExecuteOn<BasicSignal>([&executedSignals](const BasicSignal& signal)
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

        auto relic0 = savedReliquary->Do<Create<OpenRelic>>();
        relic0->Create<BasicShard>();
        relic0->Create<OtherShard>();
        auto relic1 = savedReliquary->Do<Create<OpenRelic>>();
        relic1->Create<BasicShard>();
        relic1->Create<OtherShard>();
        auto relic2 = savedReliquary->Do<Create<OpenRelic>>();
        relic2->Create<BasicShard>();
        relic2->Create<OtherShard>();

        WHEN("saving reliquary")
        {
            {
                auto output = Inscription::OutputBinaryArchive("Test.dat", "Test", 1);
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

                auto input = Inscription::InputBinaryArchive("Test.dat", "Test");
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

        auto relic0 = savedReliquary->Do<Create<OpenRelic>>();
        auto relic1 = savedReliquary->Do<Create<OpenRelic>>();
        auto relic2 = savedReliquary->Do<Create<OpenRelic>>();

        WHEN("saving reliquary")
        {
            {
                auto output = Inscription::OutputBinaryArchive("Test.dat", "Test", 1);
                output(*savedReliquary);
            }

            WHEN("loading reliquary and creating three new open relics")
            {
                const auto loadedReliquary = ReliquaryOrigin()
                    .Actualize();

                auto input = Inscription::InputBinaryArchive("Test.dat", "Test");
                input(*loadedReliquary);

                auto relic3 = loadedReliquary->Do<Create<OpenRelic>>();
                auto relic4 = loadedReliquary->Do<Create<OpenRelic>>();
                auto relic5 = loadedReliquary->Do<Create<OpenRelic>>();

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

        auto relic = reliquary->Do<Create<OpenRelic>>();
        auto shard = relic->Create<BasicShard>();

        auto setValue = dataGeneration.Random<std::string>();

        curator.onWork = [relic, setValue](DifferentiableCuratorBase& self)
        {
            auto data = self.TheData<BasicShard>(relic.ID());
            data->myValue = setValue;
        };

        reliquary->Work();

        WHEN("querying shard")
        {
            auto afterShard = relic->Find<BasicShard>();

            THEN("data has been set")
            {
                REQUIRE(afterShard->myValue == setValue);
            }
        }

        WHEN("querying index set before modification")
        {
            THEN("data has been set")
            {
                REQUIRE(shard->myValue == setValue);
            }
        }
    }
}