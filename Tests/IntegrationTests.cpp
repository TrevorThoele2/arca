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
    const auto child = Owner().CreateRelic<ChildRelic>();
    Owner().ParentRelic(ID(), child->ID());
}

void IntegrationTestsFixture::ParentRelic::DoInitialize()
{
    children = Owner().ChildRelicBatch<ChildRelic>(ID());
}

Reliquary& IntegrationTestsFixture::BasicCuratorBase::Owner()
{
    return Curator::Owner();
}

IntegrationTestsFixture::BasicCuratorBase::BasicCuratorBase(Reliquary& owner) : Curator(owner)
{
    onStartStep = [](BasicCuratorBase&) {};
    onWork = [](BasicCuratorBase&) {};
    onStopStep = [](BasicCuratorBase&) {};
}

void IntegrationTestsFixture::BasicCuratorBase::InitializeImplementation()
{
    basicSignals = Owner().SignalBatch<BasicSignal>();
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
    const TypeHandle ShardTraits<::IntegrationTestsFixture::BasicShard>::typeHandle =
        "IntegrationTestsBasicShard";

    const TypeHandle RelicTraits<::IntegrationTestsFixture::ChildRelic>::typeHandle =
        "IntegrationTestsChildRelic";

    const TypeHandle RelicTraits<::IntegrationTestsFixture::ParentRelic>::typeHandle =
        "IntegrationTestsParentRelic";

    std::optional<::IntegrationTestsFixture::ParentRelic>
        RelicTraits<::IntegrationTestsFixture::ParentRelic>::Factory(Reliquary& reliquary, int value)
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
        const auto curatorPipeline = CuratorPipeline
        {
            CuratorStage::All<BasicCurator<1>>(),
            CuratorStage::All<BasicCurator<4>, BasicCurator<3>>(),
            CuratorStage::All<BasicCurator<2>, BasicCurator<0>>()
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
            reliquary.FindCurator<BasicCurator<1>>(),
            reliquary.FindCurator<BasicCurator<4>>(),
            reliquary.FindCurator<BasicCurator<3>>(),
            reliquary.FindCurator<BasicCurator<2>>(),
            reliquary.FindCurator<BasicCurator<0>>()
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
            reliquary.RaiseSignal(BasicSignal{ value });

            reliquary.Work();

            THEN("curators encounter signals in order")
            {
                REQUIRE(encounteredSignals.size() == 5);
                for(size_t i = 0; i < curatorsInOrder.size(); ++i)
                    REQUIRE(encounteredSignals[i].curator == curatorsInOrder[i]);
            }

            THEN("signals are cleared")
            {
                const auto signalBatch = reliquary.SignalBatch<BasicSignal>();
                REQUIRE(signalBatch.IsEmpty());
            }
        }
    }
}

SCENARIO_METHOD(IntegrationTestsFixture, "working with parent-child relics through curators", "[integration][relic]")
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

        std::unordered_map<int, ParentRelic*> mappedParents;

        auto curator = reliquary.FindCurator<ParentChildCurator>();
        curator->onStartStep = [&mappedParents](BasicCuratorBase& self)
        {
            auto value = 100;
            auto parent = self.Owner().CreateRelic<ParentRelic>(value);
            parent->CreateChild();
            mappedParents.emplace(value, parent);
        };

        WHEN("working reliquary")
        {
            reliquary.Work();

            THEN("has created parent and child")
            {
                auto parents = reliquary.RelicBatch<ParentRelic>();
                auto children = reliquary.RelicBatch<ChildRelic>();

                REQUIRE(parents.begin() != parents.end());
                REQUIRE(children.begin() != children.end());

                auto parent = &*parents.begin();
                auto child = &*children.begin();

                REQUIRE(parent != nullptr);
                REQUIRE(child != nullptr);
                REQUIRE(&*parent->children.begin() == child);

                REQUIRE(!mappedParents.empty());
                REQUIRE(mappedParents[100] == parent);
            }
        }
    }
}