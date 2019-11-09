#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "IntegrationTests.h"

IntegrationTestsFixture::BasicShard::BasicShard(std::string myValue) : myValue(std::move(myValue))
{}

IntegrationTestsFixture::BasicCuratorBase::BasicCuratorBase(Reliquary& owner) : Curator(owner)
{
    onStartStep = []() {};
    onWork = []() {};
    onStopStep = []() {};
}

void IntegrationTestsFixture::BasicCuratorBase::InitializeImplementation()
{
    basicSignals = Owner().SignalBatch<BasicSignal>();
}

bool IntegrationTestsFixture::BasicCuratorBase::StartStepImplementation()
{
    onStartStep();
    return shouldStart;
}

void IntegrationTestsFixture::BasicCuratorBase::WorkImplementation()
{
    onWork();
}

void IntegrationTestsFixture::BasicCuratorBase::StopStepImplementation()
{
    onStopStep();
}

namespace Arca
{
    const TypeHandle ShardTraits<::IntegrationTestsFixture::BasicShard>::typeHandle =
        "IntegrationTestsBasicShard";
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
                curator->onWork = [curator, &encounteredSignals]()
                {
                    for (auto& signal : curator->basicSignals)
                        encounteredSignals.emplace_back(curator, signal);
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