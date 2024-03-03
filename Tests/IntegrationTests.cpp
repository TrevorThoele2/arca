#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "IntegrationTests.h"

IntegrationTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

IntegrationTestsFixture::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

void IntegrationTestsFixture::ParentRelic::Initialize(int value)
{
    this->value = value;
}

void IntegrationTestsFixture::ParentRelic::CreateChild() const
{
    Owner().Do<Arca::CreateChild<ChildRelic>>(AsHandle(*this));
}

Reliquary& IntegrationTestsFixture::BasicCuratorBase::TheOwner()
{
    return Curator::Owner();
}

std::function<void(IntegrationTestsFixture::BasicCuratorBase&)>
    IntegrationTestsFixture::BasicCuratorBase::onPostConstruct = [](BasicCuratorBase&) {};

std::function<void(IntegrationTestsFixture::BasicCuratorBase&)>
    IntegrationTestsFixture::BasicCuratorBase::onInitialize = [](BasicCuratorBase&){};

IntegrationTestsFixture::BasicCuratorBase::BasicCuratorBase()
{
    onWork = [](BasicCuratorBase&) {};
}

void IntegrationTestsFixture::BasicCuratorBase::PostConstructImplementation()
{
    onPostConstruct(*this);
    onPostConstruct = [](BasicCuratorBase&) {};
}

void IntegrationTestsFixture::BasicCuratorBase::InitializeImplementation()
{
    basicSignals = Owner().Batch<BasicSignal>();
    onInitialize(*this);
    onInitialize = [](BasicCuratorBase&) {};
}

void IntegrationTestsFixture::BasicCuratorBase::WorkImplementation(Stage& stage)
{
    if (shouldAbort)
    {
        stage.Abort();
        return;
    }

    onWork(*this);
}

namespace Arca
{
    bool Traits<::IntegrationTestsFixture::ParentRelic>::ShouldCreate(Reliquary& reliquary, int value)
    {
        return value >= 100;
    }
}

SCENARIO_METHOD(IntegrationTestsFixture, "working with signals through curators", "[integration][curator][signal]")
{
    GIVEN("registered reliquary")
    {
        const auto curatorPipeline = Pipeline
        {
            Stage::All<BasicCurator<1>>(),
            Stage::All<BasicCurator<4>, BasicCurator<3>>(),
            Stage::All<BasicCurator<2>, BasicCurator<0>>()
        };

        auto reliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<BasicCurator<0>>()
            .Register<BasicCurator<1>>()
            .Register<BasicCurator<2>>()
            .Register<BasicCurator<3>>()
            .Register<BasicCurator<4>>()
            .CuratorPipeline(curatorPipeline)
            .Actualize();

        std::vector<BasicCuratorBase*> curatorsInOrder
        {
            &reliquary->Find<BasicCurator<1>>(),
            &reliquary->Find<BasicCurator<4>>(),
            &reliquary->Find<BasicCurator<3>>(),
            &reliquary->Find<BasicCurator<2>>(),
            &reliquary->Find<BasicCurator<0>>()
        };

        WHEN("raising signal then working reliquary")
        {
            struct EncounteredSignal
            {
                BasicCuratorBase* curator;
                BasicSignal signal;

                EncounteredSignal(BasicCuratorBase* curator, BasicSignal signal) :
                    curator(curator), signal(signal)
                {}
            };
            std::vector<EncounteredSignal> encounteredSignals;

            for(auto& curator : curatorsInOrder)
            {
                curator->onWork = [&encounteredSignals](BasicCuratorBase& self)
                {
                    for (auto& signal : self.basicSignals)
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
        using ParentChildCurator = BasicCurator<0>;

        auto reliquary = ReliquaryOrigin()
            .Register<ChildRelic>()
            .Register<ParentRelic>()
            .Register<ParentChildCurator>()
            .Actualize();

        std::unordered_map<int, RelicIndex<ParentRelic>> mappedParents;

        auto& curator = reliquary->Find<ParentChildCurator>();
        curator.onWork = [&mappedParents](BasicCuratorBase& self)
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
        using FocusedCurator = BasicCurator<0>;

        std::vector<BasicSignal> executedSignals;

        FocusedCurator::onInitialize = [&executedSignals](BasicCuratorBase& curator)
        {
            curator.TheOwner().ExecuteOn<BasicSignal>([&executedSignals](const BasicSignal& signal)
                {
                    executedSignals.push_back(signal);
                });
        };

        auto reliquary = ReliquaryOrigin()
            .Register<FocusedCurator>()
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
        using FocusedCurator = BasicCurator<0>;

        Batch<All<BasicShard, OtherShard>> allBatch;

        FocusedCurator::onPostConstruct = [&allBatch](BasicCuratorBase& curator)
        {
            allBatch = curator.TheOwner().Batch<All<BasicShard, OtherShard>>();
        };

        const auto savedReliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<OtherShard>()
            .Register<FocusedCurator>()
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

            WHEN("loading reliquary and taking batch in PostConstruct")
            {
                FocusedCurator::onPostConstruct = [&allBatch](BasicCuratorBase& curator)
                {
                    allBatch = curator.TheOwner().Batch<All<BasicShard, OtherShard>>();
                };

                const auto loadedReliquary = ReliquaryOrigin()
                    .Register<BasicShard>()
                    .Register<OtherShard>()
                    .Register<FocusedCurator>()
                    .Actualize();

                FocusedCurator::onPostConstruct = [&allBatch](BasicCuratorBase& curator)
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
        using FocusedCurator = BasicCurator<0>;

        const auto reliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Register<FocusedCurator>()
            .Actualize();

        auto& curator = reliquary->Find<FocusedCurator>();

        auto relic = reliquary->Do<Create<OpenRelic>>();
        auto shard = relic->Create<BasicShard>();

        auto setValue = dataGeneration.Random<std::string>();

        curator.onWork = [relic, setValue](BasicCuratorBase& self)
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