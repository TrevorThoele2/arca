#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "HandleTests.h"

HandleTestsFixture::Shard::Shard(std::string myValue) : myValue(std::move(myValue))
{}

HandleTestsFixture::OtherShard::OtherShard(std::string myValue) : myValue(std::move(myValue))
{}

void HandleTestsFixture::TypedRelic::InitializeImplementation()
{
    auto shards = ExtractShards();
    basicShard = std::get<0>(shards);
}

void HandleTestsFixture::GlobalRelic::InitializeImplementation()
{
    auto tuple = ExtractShards();
    basicShard = std::get<0>(tuple);
}

namespace Arca
{
    const TypeHandleName Traits<::HandleTestsFixture::Shard>::typeName =
        "HandleTestsShard";

    const TypeHandleName Traits<::HandleTestsFixture::OtherShard>::typeName =
        "HandleTestsOtherShard";

    const TypeHandleName Traits<::HandleTestsFixture::TypedRelic>::typeName =
        "HandleTestsTypedRelic";

    const TypeHandleName Traits<::HandleTestsFixture::GlobalRelic>::typeName =
        "HandleTestsGlobalRelic";

    const TypeHandleName Traits<::HandleTestsFixture::Curator>::typeName =
        "HandleTestsBasicCurator";

    const TypeHandleName Traits<::HandleTestsFixture::Signal>::typeName =
        "HandleTestsBasicSignal";
}

SCENARIO_METHOD(HandleTestsFixture, "basic handle", "[handle]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Shard>()
            .Type<OtherShard>()
            .Type<TypedRelic>()
            .Type<GlobalRelic>()
            .Type<Curator>()
            .Type<Signal>()
            .Actualize();

        WHEN("creating object")
        {
            const auto relic1 = reliquary->Create<TypedRelic>();
            auto handle1 = static_cast<Handle>(relic1);

            THEN("is not equal to different ID object")
            {
                const auto relic2 = reliquary->Create<TypedRelic>();
                auto handle2 = static_cast<Handle>(relic2);

                REQUIRE(handle1 != handle2);
            }

            THEN("is not equal to same ID object with different type")
            {
                const auto shard = reliquary->Find<Shard>(relic1.ID());
                auto handle2 = static_cast<Handle>(shard);

                REQUIRE(handle1 != handle2);
            }
        }

        WHEN("creating all handles")
        {
            auto openRelic = reliquary->Create<OpenRelic>();
            auto openRelicHandle = static_cast<Handle>(openRelic);
            auto openRelicOtherShardHandle = static_cast<Handle>(openRelic.Create<OtherShard>());

            auto typedRelic = reliquary->Create<TypedRelic>();
            auto typedRelicHandle = static_cast<Handle>(typedRelic);
            auto typedRelicShardHandle = static_cast<Handle>(typedRelic->basicShard);

            auto globalRelic = reliquary->Find<GlobalRelic>();
            auto globalRelicHandle = static_cast<Handle>(globalRelic);


        }
    }
}

SCENARIO_METHOD(HandleTestsFixture, "handle combinations", "[handle]")
{
    GIVEN("all types")
    {
        auto reliquary = ReliquaryOrigin()
            .Type<Shard>()
            .Type<OtherShard>()
            .Type<TypedRelic>()
            .Type<GlobalRelic>()
            .Type<Curator>()
            .Type<Signal>()
            .Actualize();

        auto openRelic1 = reliquary->Create<OpenRelic>();
        auto openRelicHandle1 = static_cast<Handle>(openRelic1);
        auto openRelicOtherShardHandle1 = static_cast<Handle>(openRelic1.Create<OtherShard>());

        auto typedRelic1 = reliquary->Create<TypedRelic>();
        auto typedRelicHandle1 = static_cast<Handle>(typedRelic1);
        auto typedRelicShardHandle1 = static_cast<Handle>(typedRelic1->basicShard);

        auto globalRelic1 = reliquary->Find<GlobalRelic>();
        auto globalRelicHandle1 = static_cast<Handle>(globalRelic1);

        auto closedRelic1 = reliquary->Create<ClosedRelic>(RelicStructure { TypeHandleFor<Shard>() });
        auto closedRelicHandle1 = static_cast<Handle>(closedRelic1);
        auto closedRelicShardHandle1 = static_cast<Handle>(closedRelic1.Find<Shard>());

        auto openRelic2 = reliquary->Create<OpenRelic>();
        auto openRelicHandle2 = static_cast<Handle>(openRelic2);
        auto openRelicOtherShardHandle2 = static_cast<Handle>(openRelic2.Create<OtherShard>());

        auto typedRelic2 = reliquary->Create<TypedRelic>();
        auto typedRelicHandle2 = static_cast<Handle>(typedRelic2);
        auto typedRelicShardHandle2 = static_cast<Handle>(typedRelic2->basicShard);

        auto globalRelic2 = reliquary->Find<GlobalRelic>();
        auto globalRelicHandle2 = static_cast<Handle>(globalRelic2);

        WHEN("comparing open relic")
        {
            THEN("is equal to self")
            {
                REQUIRE(openRelicHandle1 == openRelicHandle1);
            }

            THEN("is not equal to open relic")
            {
                REQUIRE(openRelicHandle1 != openRelicHandle2);
            }

            THEN("is not equal to other shard")
            {
                REQUIRE(openRelicHandle1 != openRelicOtherShardHandle1);
            }

            THEN("is not equal to typed relic")
            {
                REQUIRE(openRelicHandle1 != typedRelicHandle1);
            }

            THEN("is not equal to shard")
            {
                REQUIRE(openRelicHandle1 != typedRelicShardHandle1);
            }

            THEN("is not equal to global relic")
            {
                REQUIRE(openRelicHandle1 != globalRelicHandle1);
            }

            THEN("is not equal to closed relic")
            {
                REQUIRE(openRelicHandle1 != closedRelicHandle1);
            }
        }
    }
}