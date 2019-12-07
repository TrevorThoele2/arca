#include <catch.hpp>
#include <utility>
using namespace std::string_literals;

#include "HandleTests.h"

#include <Arca/Actualization.h>

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
    const TypeName Traits<::HandleTestsFixture::Shard>::typeName =
        "HandleTestsShard";

    const TypeName Traits<::HandleTestsFixture::OtherShard>::typeName =
        "HandleTestsOtherShard";

    const TypeName Traits<::HandleTestsFixture::TypedRelic>::typeName =
        "HandleTestsTypedRelic";

    const TypeName Traits<::HandleTestsFixture::GlobalRelic>::typeName =
        "HandleTestsGlobalRelic";

    const TypeName Traits<::HandleTestsFixture::Curator>::typeName =
        "HandleTestsBasicCurator";

    const TypeName Traits<::HandleTestsFixture::Signal>::typeName =
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
    }
}

SCENARIO_METHOD(HandleTestsFixture, "handle comparison combinations", "[handle]")
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
        auto openRelicOtherShardHandle1 = static_cast<Handle>(openRelic1->Create<OtherShard>());

        auto typedRelic1 = reliquary->Create<TypedRelic>();
        auto typedRelicHandle1 = static_cast<Handle>(typedRelic1);
        auto typedRelicShardHandle1 = static_cast<Handle>(typedRelic1->basicShard);

        auto globalRelic1 = reliquary->Find<GlobalRelic>();
        auto globalRelicHandle1 = static_cast<Handle>(globalRelic1);

        auto closedRelic1 = reliquary->CreateWith<ClosedRelic>(RelicStructure { TypeFor<Shard>() });
        auto closedRelicHandle1 = static_cast<Handle>(closedRelic1);
        auto closedRelicShardHandle1 = static_cast<Handle>(closedRelic1->Find<Shard>());

        auto openRelic2 = reliquary->Create<OpenRelic>();
        auto openRelicHandle2 = static_cast<Handle>(openRelic2);
        auto openRelicOtherShardHandle2 = static_cast<Handle>(openRelic2->Create<OtherShard>());

        auto typedRelic2 = reliquary->Create<TypedRelic>();
        auto typedRelicHandle2 = static_cast<Handle>(typedRelic2);
        auto typedRelicShardHandle2 = static_cast<Handle>(typedRelic2->basicShard);

        auto globalRelic2 = reliquary->Find<GlobalRelic>();
        auto globalRelicHandle2 = static_cast<Handle>(globalRelic2);

        auto closedRelic2 = reliquary->CreateWith<ClosedRelic>(RelicStructure{ TypeFor<Shard>() });
        auto closedRelicHandle2 = static_cast<Handle>(closedRelic2);
        auto closedRelicShardHandle2 = static_cast<Handle>(closedRelic2->Find<Shard>());

        WHEN("comparing open relic")
        {
            const auto& handle = openRelicHandle1;

            THEN("is equal to self")
            {
                REQUIRE(handle == openRelicHandle1);
            }

            THEN("is not equal to open relic")
            {
                REQUIRE(handle != openRelicHandle2);
            }

            THEN("is not equal to other shard")
            {
                REQUIRE(handle != openRelicOtherShardHandle1);
            }

            THEN("is not equal to typed relic")
            {
                REQUIRE(handle != typedRelicHandle1);
            }

            THEN("is not equal to shard")
            {
                REQUIRE(handle != typedRelicShardHandle1);
            }

            THEN("is not equal to global relic")
            {
                REQUIRE(handle != globalRelicHandle1);
            }

            THEN("is not equal to closed relic")
            {
                REQUIRE(handle != closedRelicHandle1);
            }
        }

        WHEN("comparing other shard")
        {
            const auto& handle = openRelicOtherShardHandle1;

            THEN("is equal to self")
            {
                REQUIRE(handle == openRelicOtherShardHandle1);
            }

            THEN("is not equal to open relic")
            {
                REQUIRE(handle != openRelicHandle1);
            }

            THEN("is not equal to other shard")
            {
                REQUIRE(handle != openRelicOtherShardHandle2);
            }

            THEN("is not equal to typed relic")
            {
                REQUIRE(handle != typedRelicHandle1);
            }

            THEN("is not equal to shard")
            {
                REQUIRE(handle != typedRelicShardHandle1);
            }

            THEN("is not equal to global relic")
            {
                REQUIRE(handle != globalRelicHandle1);
            }

            THEN("is not equal to closed relic")
            {
                REQUIRE(handle != closedRelicHandle1);
            }
        }

        WHEN("comparing typed relic")
        {
            const auto& handle = typedRelicHandle1;

            THEN("is equal to self")
            {
                REQUIRE(handle == typedRelicHandle1);
            }

            THEN("is not equal to open relic")
            {
                REQUIRE(handle != openRelicHandle1);
            }

            THEN("is not equal to other shard")
            {
                REQUIRE(handle != openRelicOtherShardHandle1);
            }

            THEN("is not equal to typed relic")
            {
                REQUIRE(handle != typedRelicHandle2);
            }

            THEN("is not equal to shard")
            {
                REQUIRE(handle != typedRelicShardHandle1);
            }

            THEN("is not equal to global relic")
            {
                REQUIRE(handle != globalRelicHandle1);
            }

            THEN("is not equal to closed relic")
            {
                REQUIRE(handle != closedRelicHandle1);
            }
        }

        WHEN("comparing typed relic shard")
        {
            const auto& handle = typedRelicShardHandle1;

            THEN("is equal to self")
            {
                REQUIRE(handle == typedRelicShardHandle1);
            }

            THEN("is not equal to open relic")
            {
                REQUIRE(handle != openRelicHandle1);
            }

            THEN("is not equal to other shard")
            {
                REQUIRE(handle != openRelicOtherShardHandle1);
            }

            THEN("is not equal to typed relic")
            {
                REQUIRE(handle != typedRelicHandle1);
            }

            THEN("is not equal to shard")
            {
                REQUIRE(handle != typedRelicShardHandle2);
            }

            THEN("is not equal to global relic")
            {
                REQUIRE(handle != globalRelicHandle1);
            }

            THEN("is not equal to closed relic")
            {
                REQUIRE(handle != closedRelicHandle1);
            }
        }

        WHEN("comparing global relic")
        {
            const auto& handle = globalRelicHandle1;

            THEN("is equal to self")
            {
                REQUIRE(handle == globalRelicHandle1);
            }

            THEN("is not equal to open relic")
            {
                REQUIRE(handle != openRelicHandle1);
            }

            THEN("is not equal to other shard")
            {
                REQUIRE(handle != openRelicOtherShardHandle1);
            }

            THEN("is not equal to typed relic")
            {
                REQUIRE(handle != typedRelicHandle1);
            }

            THEN("is not equal to shard")
            {
                REQUIRE(handle != typedRelicShardHandle1);
            }

            THEN("is equal to global relic")
            {
                REQUIRE(handle == globalRelicHandle2);
            }

            THEN("is not equal to closed relic")
            {
                REQUIRE(handle != closedRelicHandle1);
            }
        }

        WHEN("comparing closed relic")
        {
            const auto& handle = closedRelicHandle1;

            THEN("is equal to self")
            {
                REQUIRE(handle == closedRelicHandle1);
            }

            THEN("is not equal to open relic")
            {
                REQUIRE(handle != openRelicHandle1);
            }

            THEN("is not equal to other shard")
            {
                REQUIRE(handle != openRelicOtherShardHandle1);
            }

            THEN("is not equal to typed relic")
            {
                REQUIRE(handle != typedRelicHandle1);
            }

            THEN("is not equal to shard")
            {
                REQUIRE(handle != typedRelicShardHandle1);
            }

            THEN("is not equal to global relic")
            {
                REQUIRE(handle != globalRelicHandle1);
            }

            THEN("is not equal to closed relic")
            {
                REQUIRE(handle != closedRelicHandle2);
            }
        }
    }
}

SCENARIO_METHOD(HandleTestsFixture, "handle actualizations combinations", "[handle]")
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

        auto openRelic = reliquary->Create<OpenRelic>();
        auto openRelicHandle = static_cast<Handle>(openRelic);
        auto openRelicOtherShardHandle = static_cast<Handle>(openRelic->Create<OtherShard>());

        auto typedRelic = reliquary->Create<TypedRelic>();
        auto typedRelicHandle = static_cast<Handle>(typedRelic);
        auto typedRelicShardHandle = static_cast<Handle>(typedRelic->basicShard);

        auto globalRelic = reliquary->Find<GlobalRelic>();
        auto globalRelicHandle = static_cast<Handle>(globalRelic);

        auto closedRelic = reliquary->CreateWith<ClosedRelic>(RelicStructure{ TypeFor<Shard>() });
        auto closedRelicHandle = static_cast<Handle>(closedRelic);
        auto closedRelicShardHandle = static_cast<Handle>(closedRelic->Find<Shard>());

        WHEN("actualizing open relic")
        {
            const auto& handle = openRelicHandle;

            THEN("is valid when set to open relic")
            {
                auto actualized = Actualize<OpenRelic>(handle);
                REQUIRE(actualized);
            }

            THEN("is not valid when set to other shard")
            {
                auto actualized = Actualize<OtherShard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to typed relic")
            {
                auto actualized = Actualize<TypedRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to shard")
            {
                auto actualized = Actualize<Shard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to global relic")
            {
                auto actualized = Actualize<GlobalRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to closed relic")
            {
                auto actualized = Actualize<ClosedRelic>(handle);
                REQUIRE(!actualized);
            }
        }

        WHEN("actualizing other shard")
        {
            const auto& handle = openRelicOtherShardHandle;

            THEN("is not valid when set to open relic")
            {
                auto actualized = Actualize<OpenRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is valid when set to other shard")
            {
                auto actualized = Actualize<OtherShard>(handle);
                REQUIRE(actualized);
            }

            THEN("is not valid when set to typed relic")
            {
                auto actualized = Actualize<TypedRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to shard")
            {
                auto actualized = Actualize<Shard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to global relic")
            {
                auto actualized = Actualize<GlobalRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to closed relic")
            {
                auto actualized = Actualize<ClosedRelic>(handle);
                REQUIRE(!actualized);
            }
        }

        WHEN("actualizing typed relic")
        {
            const auto& handle = typedRelicHandle;

            THEN("is not valid when set to open relic")
            {
                auto actualized = Actualize<OpenRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to other shard")
            {
                auto actualized = Actualize<OtherShard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is valid when set to typed relic")
            {
                auto actualized = Actualize<TypedRelic>(handle);
                REQUIRE(actualized);
            }

            THEN("is not valid when set to shard")
            {
                auto actualized = Actualize<Shard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to global relic")
            {
                auto actualized = Actualize<GlobalRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to closed relic")
            {
                auto actualized = Actualize<ClosedRelic>(handle);
                REQUIRE(!actualized);
            }
        }

        WHEN("actualizing shard")
        {
            const auto& handle = typedRelicShardHandle;

            THEN("is not valid when set to open relic")
            {
                auto actualized = Actualize<OpenRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to other shard")
            {
                auto actualized = Actualize<OtherShard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to typed relic")
            {
                auto actualized = Actualize<TypedRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is valid when set to shard")
            {
                auto actualized = Actualize<Shard>(handle);
                REQUIRE(actualized);
            }

            THEN("is not valid when set to global relic")
            {
                auto actualized = Actualize<GlobalRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to closed relic")
            {
                auto actualized = Actualize<ClosedRelic>(handle);
                REQUIRE(!actualized);
            }
        }

        WHEN("actualizing global relic")
        {
            const auto& handle = globalRelic;

            THEN("is not valid when set to open relic")
            {
                auto actualized = Actualize<OpenRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to other shard")
            {
                auto actualized = Actualize<OtherShard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to typed relic")
            {
                auto actualized = Actualize<TypedRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to shard")
            {
                auto actualized = Actualize<Shard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is valid when set to global relic")
            {
                auto actualized = Actualize<GlobalRelic>(handle);
                REQUIRE(actualized);
            }

            THEN("is not valid when set to closed relic")
            {
                auto actualized = Actualize<ClosedRelic>(handle);
                REQUIRE(!actualized);
            }
        }

        WHEN("actualizing closed relic")
        {
            const auto& handle = closedRelic;

            THEN("is not valid when set to open relic")
            {
                auto actualized = Actualize<OpenRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to other shard")
            {
                auto actualized = Actualize<OtherShard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to typed relic")
            {
                auto actualized = Actualize<TypedRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to shard")
            {
                auto actualized = Actualize<Shard>(handle);
                REQUIRE(!actualized);
            }

            THEN("is not valid when set to global relic")
            {
                auto actualized = Actualize<GlobalRelic>(handle);
                REQUIRE(!actualized);
            }

            THEN("is valid when set to closed relic")
            {
                auto actualized = Actualize<ClosedRelic>(handle);
                REQUIRE(actualized);
            }
        }
    }
}