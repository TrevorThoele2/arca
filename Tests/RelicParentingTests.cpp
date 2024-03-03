#include <catch.hpp>
using namespace std::string_literals;

#include "RelicParentingTests.h"
#include "SignalListener.h"

#include <Arca/OpenRelic.h>
#include <Arca/RelicParented.h>

RelicParentingTestsFixture::GlobalRelic::GlobalRelic(RelicInit init)
{
    basicShard = init.Create<BasicShard>();
}

SCENARIO_METHOD(RelicParentingTestsFixture, "relic parenting", "[relic][parenting]")
{
    GIVEN("registered reliquary")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<BasicShard>()
            .Actualize();

        auto onParented = SignalListener<RelicParented>(*reliquary);

        WHEN("creating child on nonexistent relic")
        {
            auto nonExistentRelic = Handle();

            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(CreateChild<OpenRelic>{nonExistentRelic}),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "Cannot parent a relic to a relic in a different Reliquary.")
                );
            }

            THEN("children of non existent relic from Reliquary is empty")
            {
                const auto foundChildren = reliquary->ChildrenOf(nonExistentRelic.ID());
                REQUIRE(foundChildren.empty());
            }

            THEN("does not send signal")
            {
                REQUIRE(onParented.Executions().empty());
            }
        }
    }

    GIVEN("global relic and open relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<GlobalRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto globalRelic = Arca::Index<GlobalRelic>(*reliquary);

        auto onParented = SignalListener<RelicParented>(*reliquary);

        WHEN("parenting child to global parent")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    reliquary->Do(CreateChild<OpenRelic>{AsHandle(globalRelic)}),
                    CannotParentRelic,
                    ::Catch::Matchers::Message("Cannot parent a relic to a global relic.")
                );
            }

            THEN("parent of global relic from Reliquary is null")
            {
                const auto foundParent = reliquary->ParentOf(globalRelic.ID());
                REQUIRE(!foundParent);
            }

            THEN("children of global relic from Reliquary is empty")
            {
                const auto foundChildren = reliquary->ChildrenOf(globalRelic.ID());
                REQUIRE(foundChildren.empty());
            }

            THEN("does not send signal")
            {
                REQUIRE(onParented.Executions().empty());
            }
        }
    }

    GIVEN("parent and child created")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto parent = reliquary->Do(Create<OpenRelic>());
        reliquary->Do(Create<BasicShard>(parent.ID()));

        auto onParented = SignalListener<RelicParented>(*reliquary);

        WHEN("created child")
        {
            auto child = reliquary->Do(CreateChild<OpenRelic>{AsHandle(parent)});
            reliquary->Do(Create<BasicShard>(child.ID()));

            THEN("parent of child from Reliquary is parent")
            {
                const auto foundParent = reliquary->ParentOf(child.ID());
                REQUIRE(foundParent);
                REQUIRE(foundParent->ID() == parent.ID());
            }

            THEN("children of parent from Reliquary is child")
            {
                const auto foundChildren = reliquary->ChildrenOf(parent.ID());
                REQUIRE(foundChildren.size() == 1);
                REQUIRE(foundChildren[0].ID() == child.ID());
            }

            THEN("destroying parent also destroys child")
            {
                reliquary->Destroy(AsHandle(parent));

                REQUIRE(reliquary->RelicSize() == 0);
                REQUIRE(!Arca::Index<BasicShard>(child.ID(), *reliquary));
            }

            THEN("sends signal")
            {
                REQUIRE(!onParented.Executions().empty());

                auto signal = onParented.Executions().begin();

                REQUIRE(signal->parent.ID() == parent.ID());
                REQUIRE(signal->child.ID() == child.ID());
            }

            WHEN("destroying child")
            {
                reliquary->Destroy(AsHandle(child));

                THEN("children of parent from Reliquary is empty")
                {
                    const auto foundChildren = reliquary->ChildrenOf(parent.ID());
                    REQUIRE(foundChildren.empty());
                }

                THEN("does not destroy parent")
                {
                    REQUIRE(reliquary->RelicSize() == 1);
                    REQUIRE(Arca::Index<BasicShard>(parent.ID(), *reliquary));
                }

                THEN("destroying parent works")
                {
                    reliquary->Destroy(AsHandle(parent));

                    REQUIRE(reliquary->RelicSize() == 0);
                    REQUIRE(!Arca::Index<BasicShard>(parent.ID(), *reliquary));
                }
            }
        }
    }

    GIVEN("multiple children parented to same relic")
    {
        auto reliquary = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>()
            .Actualize();

        auto parent = reliquary->Do(Create<OpenRelic>());
        reliquary->Do(Create<BasicShard>(parent.ID()));

        auto onParented = SignalListener<RelicParented>(*reliquary);

        WHEN("created 10 children")
        {
            std::vector<Index<OpenRelic>> children;
            for (auto i = 0; i < 10; ++i)
            {
                auto child = reliquary->Do(CreateChild<OpenRelic>{AsHandle(parent)});
                reliquary->Do(Create<BasicShard>(child.ID()));
                children.push_back(child);
            }

            THEN("parent of children from Reliquary is parent")
            {
                for (auto& child : children)
                {
                    const auto foundParent = reliquary->ParentOf(child.ID());
                    REQUIRE(foundParent);
                    REQUIRE(foundParent->ID() == parent.ID());
                }
            }

            THEN("children of parent from Reliquary is child")
            {
                const auto foundChildren = reliquary->ChildrenOf(parent.ID());
                REQUIRE(foundChildren.size() == children.size());
                for (auto i = 0; i < 10; ++i)
                    REQUIRE(foundChildren[i].ID() == children[i].ID());
            }

            THEN("destroying parent also destroys children")
            {
                reliquary->Destroy(AsHandle(parent));

                REQUIRE(reliquary->RelicSize() == 0);
                for (auto& child : children)
                    REQUIRE(!Arca::Index<BasicShard>(child.ID(), *reliquary));
            }

            THEN("sends 10 signals")
            {
                REQUIRE(onParented.Executions().size() == 10);

                for (auto i = 0; i < 10; ++i)
                {
                    auto& signal = onParented.Executions()[i];
                    auto& child = children[i];
                    REQUIRE(signal.parent.ID() == parent.ID());
                    REQUIRE(signal.child.ID() == child.ID());
                }
            }

            WHEN("destroying child")
            {
                reliquary->Destroy(AsHandle(children[4]));

                auto checkChildren = children;
                children.erase(children.begin() + 4);

                THEN("children of parent from Reliquary is 9")
                {
                    const auto foundChildren = reliquary->ChildrenOf(parent.ID());
                    REQUIRE(foundChildren.size() == 9);
                    for (auto i = 0; i < 9; ++i)
                        REQUIRE(foundChildren[i].ID() == children[i].ID());
                }

                THEN("does not destroy parent")
                {
                    REQUIRE(reliquary->RelicSize() == 10);
                    REQUIRE(Arca::Index<BasicShard>(parent.ID(), *reliquary));
                }

                THEN("destroying parent works")
                {
                    reliquary->Destroy(AsHandle(parent));

                    REQUIRE(reliquary->RelicSize() == 0);
                    for (auto& child : children)
                        REQUIRE(!Arca::Index<BasicShard>(child.ID(), *reliquary));
                }
            }
        }
    }

    GIVEN("parent from one reliquary and child from another")
    {
        auto origin = ReliquaryOrigin()
            .Register<OpenRelic>()
            .Register<BasicShard>();

        auto parentReliquary = origin.Actualize();
        auto childReliquary = origin.Actualize();

        auto parent = parentReliquary->Do(Create<OpenRelic>());
        parentReliquary->Do(Create<BasicShard>(parent.ID()));

        WHEN("parenting child inside child reliquary")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    childReliquary->Do(CreateChild<OpenRelic>{AsHandle(parent)}),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "Cannot parent a relic to a relic in a different Reliquary.")
                );
            }

            THEN("children of parent from parent Reliquary is empty")
            {
                const auto foundChildren = parentReliquary->ChildrenOf(parent.ID());
                REQUIRE(foundChildren.empty());
            }
        }

        WHEN("parenting child inside irrelevant reliquary")
        {
            auto irrelevantReliquary = origin.Actualize();

            THEN("throws error")
            {
                REQUIRE_THROWS_MATCHES
                (
                    irrelevantReliquary->Do(CreateChild<OpenRelic>{AsHandle(parent)}),
                    CannotParentRelic,
                    ::Catch::Matchers::Message(
                        "Cannot parent a relic to a relic in a different Reliquary.")
                );
            }

            THEN("children of parent from parent Reliquary is empty")
            {
                const auto foundChildren = parentReliquary->ChildrenOf(parent.ID());
                REQUIRE(foundChildren.empty());
            }
        }
    }
}