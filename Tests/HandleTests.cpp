#include <catch.hpp>
using namespace std::string_literals;

#include "HandleTests.h"

#include <Arca/OpenRelic.h>

HandleTestsFixture::TypedRelic::TypedRelic(RelicInit init)
{
    basicShard = init.Create<Shard>();
}

HandleTestsFixture::TypedRelic::TypedRelic(RelicInit init, Serialization)
{
    basicShard = init.Find<Shard>();
}

HandleTestsFixture::GlobalRelic::GlobalRelic(RelicInit init)
{
    basicShard = init.Create<Shard>();
}

HandleTestsFixture::HandleHolder::HandleHolder(RelicInit init)
{}

HandleTestsFixture::HandleHolder::HandleHolder(RelicInit init, Handle handle) : handle(handle)
{}

SCENARIO_METHOD(HandleTestsFixture, "basic handle", "[handle]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Register<OtherShard>()
            .Register<TypedRelic>()
            .Register<GlobalRelic>()
            .Register<Curator>()
            .Actualize();

        WHEN("creating object")
        {
            auto relic1 = reliquary->Do(Create<TypedRelic>());
            auto handle1 = AsHandle(relic1);

            THEN("is not equal to different ID object")
            {
                auto relic2 = reliquary->Do(Create<TypedRelic>());
                auto handle2 = AsHandle(relic2);

                REQUIRE(handle1 != handle2);
            }

            THEN("is not equal to same ID object with different type")
            {
                auto handle2 = AsHandle<Shard>(relic1.ID());

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
            .Register<OpenRelic>()
            .Register<Shard>()
            .Register<OtherShard>()
            .Register<TypedRelic>()
            .Register<GlobalRelic>()
            .Register<Curator>()
            .Actualize();

        auto openRelic1 = reliquary->Do(Create<OpenRelic>());
        auto openRelicHandle1 = AsHandle(openRelic1);
        reliquary->Do(Create<OtherShard>(openRelic1.ID()));
        auto openRelicOtherShardHandle1 = AsHandle<OtherShard>(openRelic1.ID());

        auto typedRelic1 = reliquary->Do(Create<TypedRelic>());
        auto typedRelicHandle1 = AsHandle(typedRelic1);
        auto typedRelicShardHandle1 = AsHandle<Shard>(typedRelic1.ID());

        auto globalRelic1 = reliquary->Find<GlobalRelic>();
        auto globalRelicHandle1 = AsHandle(globalRelic1);

        auto openRelic2 = reliquary->Do(Create<OpenRelic>());
        auto openRelicHandle2 = AsHandle(openRelic2);
        reliquary->Do(Create<OtherShard>(openRelic2.ID()));
        auto openRelicOtherShardHandle2 = AsHandle<OtherShard>(openRelic2.ID());

        auto typedRelic2 = reliquary->Do(Create<TypedRelic>());
        auto typedRelicHandle2 = AsHandle(typedRelic2);
        auto typedRelicShardHandle2 = AsHandle<Shard>(typedRelic2.ID());

        auto globalRelic2 = reliquary->Find<GlobalRelic>();
        auto globalRelicHandle2 = AsHandle(globalRelic2);

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
        }
    }
}

SCENARIO_METHOD(HandleTestsFixture, "handle serialization", "[handle][serialization]")
{
    GIVEN("relic registered")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<Shard>()
            .Register<TypedRelic>()
            .Register<HandleHolder>()
            .Actualize();

        const auto index = savedReliquary->Do(Arca::Create<TypedRelic>());
        const auto savedHandleHolder = savedReliquary->Do(Arca::Create<HandleHolder>(static_cast<Handle>(index)));

        WHEN("saving handle")
        {
            {
                auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
                outputArchive(*savedReliquary);
            }

            THEN("loading handle contains same information")
            {
                auto loadedReliquary = ReliquaryOrigin()
                    .Register<Shard>()
                    .Register<TypedRelic>()
                    .Register<HandleHolder>()
                    .Actualize();

                {
                    auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                    inputArchive(*loadedReliquary);
                }

                auto loadedHandleHolder = loadedReliquary->Find<HandleHolder>(savedHandleHolder.ID());

                REQUIRE(loadedHandleHolder->handle.id == savedHandleHolder->handle.id);
                REQUIRE(loadedHandleHolder->handle.type == savedHandleHolder->handle.type);
            }
        }
    }

    GIVEN("shard registered")
    {
        auto savedReliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<Shard>()
            .Register<HandleHolder>()
            .Actualize();

        const auto relic = savedReliquary->Do(Arca::Create<OpenRelic>());
        auto index = savedReliquary->Do(Create<Shard>(relic.ID()));
        const auto savedHandleHolder = savedReliquary->Do(Arca::Create<HandleHolder>(index));

        WHEN("saving handle")
        {
            {
                auto outputArchive = ::Inscription::Archive::OutputBinary("Test.dat");
                outputArchive(*savedReliquary);
            }

            THEN("loading handle contains same information")
            {
                auto loadedReliquary = ReliquaryOrigin()
                    .Register<OpenRelic>()
                    .Register<Shard>()
                    .Register<TypedRelic>()
                    .Register<HandleHolder>()
                    .Actualize();

                Handle loadedHandle;

                {
                    auto inputArchive = ::Inscription::Archive::InputBinary("Test.dat");
                    inputArchive(*loadedReliquary);
                }

                auto loadedHandleHolder = loadedReliquary->Find<HandleHolder>(savedHandleHolder.ID());

                REQUIRE(loadedHandleHolder->handle.id == savedHandleHolder->handle.id);
                REQUIRE(loadedHandleHolder->handle.type == savedHandleHolder->handle.type);
            }
        }
    }
}