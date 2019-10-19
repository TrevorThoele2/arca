#include <catch.hpp>
#include <utility>

#include "ReliquaryTests.h"

ReliquaryTestsFixture::ReliquaryTestsFixture()
{
    auto typeGroup = typeRegistration.CreateGroup();
    typeGroup->RegisterRelic<BasicRelic>();
    typeGroup->RegisterRelic<CustomFactoryRelic>();
    typeGroup->RegisterRelic<AbstractRelic>();
    typeGroup->RegisterRelic<DerivedRelic>();
}

ReliquaryTestsFixture::BasicRelic::BasicRelic(std::string myValue) : myValue(std::move(myValue))
{}

ReliquaryTestsFixture::BasicRelic::BasicRelic(const ::Inscription::BinaryTableData<BasicRelic>& data) :
    myValue(data.myValue)
{}

ReliquaryTestsFixture::StaticRelic::StaticRelic(std::string myValue) : myValue(std::move(myValue))
{}

ReliquaryTestsFixture::StaticRelic::StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data) :
    myValue(data.myValue)
{}

ReliquaryTestsFixture::CustomFactoryRelic::CustomFactoryRelic(std::string factorySupplied) : 
    factorySupplied(std::move(factorySupplied))
{}

ReliquaryTestsFixture::CustomFactoryRelic::CustomFactoryRelic(
    std::string factorySupplied, std::string myValue)
    :
    myValue(std::move(myValue)), factorySupplied(std::move(factorySupplied))
{}

ReliquaryTestsFixture::CustomFactoryRelic::CustomFactoryRelic(
    const ::Inscription::BinaryTableData<CustomFactoryRelic>& data
) {}

ReliquaryTestsFixture::AbstractRelic::AbstractRelic(const std::string& abstractValue) :
    abstractValue(abstractValue)
{}

ReliquaryTestsFixture::AbstractRelic::~AbstractRelic() = default;

ReliquaryTestsFixture::AbstractRelic::AbstractRelic(const ::Inscription::BinaryTableData<AbstractRelic>& data)
{}

ReliquaryTestsFixture::DerivedRelic::DerivedRelic(const std::string& abstractValue, const std::string& derivedValue) :
    AbstractRelic(abstractValue), derivedValue(derivedValue)
{}

ReliquaryTestsFixture::DerivedRelic::DerivedRelic(const ::Inscription::BinaryTableData<DerivedRelic>& data) :
    AbstractRelic(std::get<0>(data.bases))
{}

ReliquaryTestsFixture::BasicCurator::BasicCurator(Reliquary& owner) : Curator(owner)
{}

namespace Arca
{
    std::string CustomFactory::factorySupplied;

    auto CustomFactory::CreateRelic(const std::string& myValue) -> Created
    {
        return Relic{ factorySupplied, myValue };
    }
}

namespace Arca
{
    const TypeHandle RelicTraits<::ReliquaryTestsFixture::BasicRelic>::typeHandle =
        "ReliquaryTestsBasicRelic";

    const TypeHandle RelicTraits<::ReliquaryTestsFixture::StaticRelic>::typeHandle =
        "ReliquaryTestsStaticRelic";

    const TypeHandle RelicTraits<::ReliquaryTestsFixture::CustomFactoryRelic>::typeHandle =
        "ReliquaryTestsCustomFactoryRelic";

    const TypeHandle RelicTraits<::ReliquaryTestsFixture::AbstractRelic>::typeHandle =
        "ReliquaryTestsAbstractRelic";

    const TypeHandle RelicTraits<::ReliquaryTestsFixture::DerivedRelic>::typeHandle =
        "ReliquaryTestsDerivedRelic";

    const TypeHandle CuratorTraits<::ReliquaryTestsFixture::BasicCurator>::typeHandle =
        "ReliquaryTestsBasicCurator";
}

SCENARIO_METHOD(ReliquaryTestsFixture, "Reliquary errors")
{
    GIVEN("initialized")
    {
        Reliquary reliquary;
        reliquary.Initialize();

        WHEN("registering non-static relic")
        {
            THEN("cannot register")
            {
                REQUIRE_THROWS_AS(reliquary.RegisterRelicType<BasicRelic>(), CannotRegister);
            }
        }

        WHEN("registering static relic")
        {
            THEN("cannot register")
            {
                REQUIRE_THROWS_AS(reliquary.RegisterRelicType<StaticRelic>(), CannotRegister);
            }
        }

        WHEN("registering curator")
        {
            THEN("cannot register")
            {
                REQUIRE_THROWS_AS(reliquary.RegisterCuratorType<BasicCurator>(), CannotRegister);
            }
        }

        WHEN("registering curator layout")
        {
            THEN("cannot register")
            {
                REQUIRE_THROWS_AS(reliquary.RegisterCuratorLayout(CuratorLayout()), CannotRegister);
            }
        }

        WHEN("registering signal")
        {
            THEN("cannot register")
            {
                REQUIRE_THROWS_AS(reliquary.RegisterSignalType<BasicSignal>(), CannotRegister);
            }
        }

        WHEN("saving and loading reliquary")
        {
            {
                auto outputArchive = CreateRegistered<::Inscription::OutputBinaryArchive>();
                outputArchive(reliquary);
            }

            THEN("throws error")
            {
                Reliquary loadedReliquary;
                loadedReliquary.Initialize();
                auto inputArchive = CreateRegistered<::Inscription::InputBinaryArchive>();
                REQUIRE_THROWS_AS(inputArchive(loadedReliquary), AttemptedLoadWhileInitialized);
            }
        }
    }

    GIVEN("uninitialized reliquary")
    {
        Reliquary reliquary;

        WHEN("checking relic count")
        {
            THEN("has relic count of zero")
            {
                REQUIRE(reliquary.RelicCount() == 0);
            }
        }

        WHEN("creating relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.CreateRelic<BasicRelic>(), NotInitialized);
            }
        }

        WHEN("retrieving static relic")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.StaticRelic<StaticRelic>(), NotInitialized);
            }
        }

        WHEN("finding curator")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.FindCurator<BasicCurator>(), NotInitialized);
            }
        }

        WHEN("raising signal")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.RaiseSignal(BasicSignal()), NotInitialized);
            }
        }
    }

    GIVEN("all registered")
    {
        Reliquary reliquary;
        reliquary.RegisterRelicType<BasicRelic>();
        reliquary.RegisterRelicType<StaticRelic>();
        reliquary.RegisterCuratorType<BasicCurator>();
        reliquary.RegisterCuratorLayout(CuratorLayout());
        reliquary.RegisterSignalType<BasicSignal>();

        WHEN("registering non-static relic again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.RegisterRelicType<BasicRelic>(), AlreadyRegistered);
            }
        }

        WHEN("registering static relic again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.RegisterRelicType<StaticRelic>(), AlreadyRegistered);
            }
        }

        WHEN("registering curator again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.RegisterCuratorType<BasicCurator>(), AlreadyRegistered);
            }
        }

        WHEN("registering curator layout again")
        {
            THEN("does not throw error")
            {
                REQUIRE_NOTHROW(reliquary.RegisterCuratorLayout(CuratorLayout()));
            }
        }

        WHEN("registering signal again")
        {
            THEN("throws error")
            {
                REQUIRE_THROWS_AS(reliquary.RegisterSignalType<BasicSignal>(), AlreadyRegistered);
            }
        }
    }
}

SCENARIO_METHOD(ReliquaryTestsFixture, "Reliquary")
{
    GIVEN("all registered types pushed and initialized")
    {
        Reliquary reliquary;
        typeRegistration.PushAllTo(reliquary);
        reliquary.Initialize();

        WHEN("creating custom factory relic")
        {
            auto strings = dataGeneration.RandomGroup<std::string>(2);

            CustomFactory::factorySupplied = strings[0];
            auto created = reliquary.CreateRelic<CustomFactoryRelic>(strings[1]);

            THEN("is constructed through custom factory")
            {
                REQUIRE(created->factorySupplied == strings[0]);
                REQUIRE(created->myValue == strings[1]);
            }
        }

        WHEN("creating three unspecified-id relics")
        {
            auto strings = dataGeneration.RandomGroup<std::string>(3);

            Ref<BasicRelic> createdRelics[3] =
            {
                reliquary.CreateRelic<BasicRelic>(strings[0]),
                reliquary.CreateRelic<BasicRelic>(strings[1]),
                reliquary.CreateRelic<BasicRelic>(strings[2])
            };

            THEN("reliquary has relic count of three")
            {
                REQUIRE(reliquary.RelicCount() == 3);
            }

            WHEN("destroying created relics")
            {
                reliquary.DestroyRelic(createdRelics[0]);
                reliquary.DestroyRelic(createdRelics[1]);
                reliquary.DestroyRelic(createdRelics[2]);

                THEN("reliquary has relic count of zero")
                {
                    REQUIRE(reliquary.RelicCount() == 0);
                }
            }
        }

        WHEN("creating derived relic")
        {
            auto strings = dataGeneration.RandomGroup<std::string>(2);

            auto created = reliquary.CreateRelic<DerivedRelic>(strings[0], strings[1]);

            THEN("created relic has correct data")
            {
                REQUIRE(created->abstractValue == strings[0]);
                REQUIRE(created->derivedValue == strings[1]);
            }
        }
    }

    GIVEN("static relic registered and initialized")
    {
        Reliquary reliquary;
        reliquary.RegisterRelicType<StaticRelic>();
        reliquary.Initialize();

        WHEN("finding static relic")
        {
            THEN("returns not null")
            {
                auto staticRelic = reliquary.StaticRelic<StaticRelic>();
                REQUIRE(staticRelic != nullptr);
            }
        }
    }
}