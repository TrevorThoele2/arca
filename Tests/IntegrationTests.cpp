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

void IntegrationTestsFixture::ParentRelic::CreateChild()
{
    Owner().CreateChild<ChildRelic>(AsHandle(*this));
}

Reliquary& IntegrationTestsFixture::BasicCuratorBase::Owner()
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
            .Type<BasicShard>()
            .Type<BasicCurator<0>>()
            .Type<BasicCurator<1>>()
            .Type<BasicCurator<2>>()
            .Type<BasicCurator<3>>()
            .Type<BasicCurator<4>>()
            .CuratorPipeline(curatorPipeline)
            .Type<BasicSignal>()
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
            .Type<ChildRelic>()
            .Type<ParentRelic>()
            .Type<BasicSignal>()
            .Type<ParentChildCurator>()
            .Actualize();

        std::unordered_map<int, ParentRelic*> mappedParents;

        auto& curator = reliquary->Find<ParentChildCurator>();
        curator.onWork = [&mappedParents](BasicCuratorBase& self)
        {
            auto value = 100;
            auto parent = self.Owner().Create<ParentRelic>(value);
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
                REQUIRE(static_cast<ParentRelic*>(mappedParents[100]) == parent);
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
            curator.Owner().ExecuteOn<BasicSignal>([&executedSignals](const BasicSignal& signal)
                {
                    executedSignals.push_back(signal);
                });
        };

        auto reliquary = ReliquaryOrigin()
            .Type<BasicSignal>()
            .Type<FocusedCurator>()
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
    "composite shard batch with either in it is occupied after loading",
    "[integration][curator][composite][either][serialization]")
{
    GIVEN("registered reliquary and 3 relics with both shards")
    {
        using FocusedCurator = BasicCurator<0>;

        Batch<All<BasicShard, OtherShard>> compositeBatch;

        FocusedCurator::onPostConstruct = [&compositeBatch](BasicCuratorBase& curator)
        {
            compositeBatch = curator.Owner().Batch<All<BasicShard, OtherShard>>();
        };

        const auto savedReliquary = ReliquaryOrigin()
            .Type<BasicShard>()
            .Type<OtherShard>()
            .Type<BasicSignal>()
            .Type<FocusedCurator>()
            .Actualize();

        auto relic0 = savedReliquary->Create<OpenRelic>();
        relic0->Create<BasicShard>();
        relic0->Create<OtherShard>();
        auto relic1 = savedReliquary->Create<OpenRelic>();
        relic1->Create<BasicShard>();
        relic1->Create<OtherShard>();
        auto relic2 = savedReliquary->Create<OpenRelic>();
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
                FocusedCurator::onPostConstruct = [&compositeBatch](BasicCuratorBase& curator)
                {
                    compositeBatch = curator.Owner().Batch<All<BasicShard, OtherShard>>();
                };

                const auto loadedReliquary = ReliquaryOrigin()
                    .Type<BasicShard>()
                    .Type<OtherShard>()
                    .Type<BasicSignal>()
                    .Type<FocusedCurator>()
                    .Actualize();

                FocusedCurator::onPostConstruct = [&compositeBatch](BasicCuratorBase& curator)
                {
                    compositeBatch = curator.Owner().Batch<All<BasicShard, OtherShard>>();
                };

                auto input = Inscription::InputBinaryArchive("Test.dat", "Test");
                input(*loadedReliquary);

                THEN("batch is occupied")
                {
                    REQUIRE(compositeBatch.Size() == 3);
                }
            }
        }
    }
}