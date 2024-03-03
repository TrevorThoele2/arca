#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "IntegrationTests.h"

IntegrationTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

IntegrationTestsFixture::ParentRelic::ParentRelic(int value) : value(value)
{}

void IntegrationTestsFixture::ParentRelic::CreateChild()
{
    Owner().CreateChild<ChildRelic>(*this);
}

Reliquary& IntegrationTestsFixture::BasicCuratorBase::Owner()
{
    return Curator::Owner();
}

std::function<void(IntegrationTestsFixture::BasicCuratorBase&)>
    IntegrationTestsFixture::BasicCuratorBase::onInitialize = [](BasicCuratorBase&){};

IntegrationTestsFixture::BasicCuratorBase::BasicCuratorBase()
{
    onStartStep = [](BasicCuratorBase&) {};
    onWork = [](BasicCuratorBase&) {};
    onStopStep = [](BasicCuratorBase&) {};
}

void IntegrationTestsFixture::BasicCuratorBase::InitializeImplementation()
{
    basicSignals = Owner().Batch<BasicSignal>();
    onInitialize(*this);
    onInitialize = [](BasicCuratorBase&) {};
}

bool IntegrationTestsFixture::BasicCuratorBase::StartStepImplementation()
{
    onStartStep(*this);
    return shouldStart;
}

void IntegrationTestsFixture::BasicCuratorBase::WorkImplementation()
{
    onWork(*this);
}

void IntegrationTestsFixture::BasicCuratorBase::StopStepImplementation()
{
    onStopStep(*this);
}

namespace Arca
{
    const TypeHandleName Traits<::IntegrationTestsFixture::BasicShard>::typeName =
        "IntegrationTestsBasicShard";

    const TypeHandleName Traits<::IntegrationTestsFixture::BasicSignal>::typeName =
        "IntegrationTestsBasicSignal";

    const TypeHandleName Traits<::IntegrationTestsFixture::ChildRelic>::typeName =
        "IntegrationTestsChildRelic";

    const TypeHandleName Traits<::IntegrationTestsFixture::ParentRelic>::typeName =
        "IntegrationTestsParentRelic";

    std::optional<::IntegrationTestsFixture::ParentRelic>
        Traits<::IntegrationTestsFixture::ParentRelic>::Factory(Reliquary& reliquary, int value)
    {
        if (value < 100)
            return {};

        return ::IntegrationTestsFixture::ParentRelic(value);
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
            .Shard<BasicShard>()
            .Curator<BasicCurator<0>>()
            .Curator<BasicCurator<1>>()
            .Curator<BasicCurator<2>>()
            .Curator<BasicCurator<3>>()
            .Curator<BasicCurator<4>>()
            .CuratorPipeline(curatorPipeline)
            .Signal<BasicSignal>()
            .Actualize();

        std::vector<BasicCuratorBase*> curatorsInOrder
        {
            reliquary->Find<BasicCurator<1>>(),
            reliquary->Find<BasicCurator<4>>(),
            reliquary->Find<BasicCurator<3>>(),
            reliquary->Find<BasicCurator<2>>(),
            reliquary->Find<BasicCurator<0>>()
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
            .Relic<ChildRelic>()
            .Relic<ParentRelic>()
            .Signal<BasicSignal>()
            .Curator<ParentChildCurator>()
            .Actualize();

        std::unordered_map<int, Ptr<ParentRelic>> mappedParents;

        auto curator = reliquary->Find<ParentChildCurator>();
        curator->onStartStep = [&mappedParents](BasicCuratorBase& self)
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
            .Signal<BasicSignal>()
            .Curator<FocusedCurator>()
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