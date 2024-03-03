#include <catch.hpp>

#include "IndexTests.h"

#include <Arca/ReliquaryOrigin.h>

#include "DifferentiableShard.h"

IndexTestsFixture::TypedClosedRelic::TypedClosedRelic(RelicInit init)
{
    shard = init.Create<BasicShard>();
}

IndexTestsFixture::TypedOpenRelic::TypedOpenRelic(RelicInit init)
{
    shard = init.Create<BasicShard>();
}

IndexTestsFixture::GlobalRelic::GlobalRelic(RelicInit init)
{
    shard = init.Create<BasicShard>();
}

IndexTestsFixture::RelicHolderRelic::RelicHolderRelic(RelicInit init)
{}

IndexTestsFixture::RelicHolderRelic::RelicHolderRelic(RelicInit init, Index<OpenRelic> held) :
    held(held)
{}

SCENARIO_METHOD(IndexTestsFixture, "Index resets to nullptr after underlying object destroyed", "[index]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Register<TypedClosedRelic>()
            .Register<TypedOpenRelic>()
            .Actualize();

        WHEN("creating open relic then destroying relic")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());

            reliquary->Destroy(relic);

            THEN("relic is nullptr")
            {
                REQUIRE(relic.Get() == nullptr);
            }

            THEN("ID is 1")
            {
                REQUIRE(relic.ID() == 1);
            }

            THEN("owner is reliquary")
            {
                REQUIRE(relic.Owner() == reliquary.get());
            }
        }
        
        WHEN("creating typed closed relic then destroying relic")
        {
            auto relic = reliquary->Do(Create<TypedClosedRelic>());

            reliquary->Destroy(relic);

            THEN("relic is nullptr")
            {
                REQUIRE(relic.Get() == nullptr);
            }

            THEN("ID is 1")
            {
                REQUIRE(relic.ID() == 1);
            }

            THEN("owner is reliquary")
            {
                REQUIRE(relic.Owner() == reliquary.get());
            }
        }

        WHEN("creating typed open relic then destroying relic")
        {
            auto relic = reliquary->Do(Create<TypedOpenRelic>());

            reliquary->Destroy(relic);

            THEN("relic is nullptr")
            {
                REQUIRE(relic.Get() == nullptr);
            }

            THEN("ID is 1")
            {
                REQUIRE(relic.ID() == 1);
            }

            THEN("owner is reliquary")
            {
                REQUIRE(relic.Owner() == reliquary.get());
            }
        }
    }
}

SCENARIO_METHOD(IndexTestsFixture, "Matrix Index", "[index][matrix]")
{
    GIVEN("registered reliquary, open relic and three shards")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<DifferentiableShard<0>>()
            .Register<DifferentiableShard<1>>()
            .Register<DifferentiableShard<2>>()
            .Actualize();

        auto relic = reliquary->Do(Create<OpenRelic>());
        auto shard0 = reliquary->Do(Create<DifferentiableShard<0>>(relic));
        auto shard1 = reliquary->Do(Create<DifferentiableShard<1>>(relic));
        auto shard2 = reliquary->Do(Create<DifferentiableShard<2>>(relic));

        WHEN("creating Index out of all")
        {
            auto index = reliquary->Find<All<
                DifferentiableShard<0>,
                DifferentiableShard<1>,
                DifferentiableShard<2>>>
                (relic.ID());

            THEN("dereferencing results in tuple of all, valid indexs")
            {
                auto tuple = *index;
                auto index0 = std::get<0>(tuple);
                auto index1 = std::get<1>(tuple);
                auto index2 = std::get<2>(tuple);
                REQUIRE(index0);
                REQUIRE(index1);
                REQUIRE(index2);
                REQUIRE(shard0 == index0);
                REQUIRE(shard1 == index1);
                REQUIRE(shard2 == index2);
            }

            THEN("is true")
            {
                REQUIRE(index);
            }

            THEN("id is same as relic")
            {
                REQUIRE(index.ID() == relic.ID());
            }

            THEN("owner is same as relic")
            {
                REQUIRE(index.Owner() == relic.Owner());
            }
        }
    }
}

SCENARIO_METHOD(IndexTestsFixture, "Index moving", "[index]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Register<TypedClosedRelic>()
            .Register<TypedOpenRelic>()
            .Register<GlobalRelic>()
            .Actualize();

        WHEN("creating open relic then move assigning from index")
        {
            auto index1 = reliquary->Do(Create<OpenRelic>());

            auto index2 = std::move(index1);

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }

        WHEN("creating open relic then move constructing from index")
        {
            auto index1 = reliquary->Do(Create<OpenRelic>());

            auto index2(std::move(index1));

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }

        WHEN("creating shard then move assigning from index")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());

            auto index1 = reliquary->Do(Create<BasicShard>(relic));

            auto index2 = std::move(index1);

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }

        WHEN("creating shard then move constructing from index")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());

            auto index1 = reliquary->Do(Create<BasicShard>(relic));

            auto index2(std::move(index1));

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }

        WHEN("creating const shard then move assigning from index")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());

            auto index1 = reliquary->Do(Create<const BasicShard>(relic));

            auto index2 = std::move(index1);

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }

        WHEN("creating const shard then move constructing from index")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());

            auto index1 = reliquary->Do(Create<const BasicShard>(relic));

            auto index2(std::move(index1));

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }

        WHEN("move assigning from global index")
        {
            auto index1 = reliquary->Find<GlobalRelic>();

            auto index2 = std::move(index1);

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }

        WHEN("move constructing from global index")
        {
            auto index1 = reliquary->Find<GlobalRelic>();

            auto index2(std::move(index1));

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }

        WHEN("creating shard then move assigning from matrix index")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());

            reliquary->Do(Create<BasicShard>(relic));

            auto index1 = reliquary->Find<Either<BasicShard>>(relic.ID());

            auto index2 = std::move(index1);

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }

        WHEN("creating shard then move constructing from matrix index")
        {
            auto relic = reliquary->Do(Create<OpenRelic>());

            reliquary->Do(Create<BasicShard>(relic));

            auto index1 = reliquary->Find<Either<BasicShard>>(relic.ID());

            auto index2(std::move(index1));

            THEN("index1 is nullptr")
            {
                REQUIRE(index1.Get() == nullptr);
            }

            THEN("index1 owner is nullptr")
            {
                REQUIRE(index1.Owner() == nullptr);
            }

            THEN("index1 is false")
            {
                REQUIRE(static_cast<bool>(index1) == false);
            }

            THEN("index2 is not nullptr")
            {
                REQUIRE(index2.Get() != nullptr);
            }

            THEN("index2 owner is reliquary")
            {
                REQUIRE(index2.Owner() == reliquary.get());
            }

            THEN("index2 is true")
            {
                REQUIRE(static_cast<bool>(index2) == true);
            }
        }
    }
}