#include <catch.hpp>
#include <utility>

#include <Inscription/MemoryScribe.h>
#include <Inscription/UnorderedMapScribe.h>

#include "GeneralFixture.h"

#include <Arca/IDManager.h>

using namespace Arca;

class IDManagerTestsFixture : public GeneralFixture
{
public:
    class Base
    {
    public:
        int baseValue = 0;
    public:
        Base() = default;

        explicit Base(int baseValue) : baseValue(baseValue)
        {}

        virtual ~Base() = 0;
    };

    class Derived : public Base
    {
    public:
        std::string derivedValue;
    public:
        Derived() = default;

        Derived(int baseValue, std::string derivedValue) : Base(baseValue), derivedValue(std::move(derivedValue))
        {}
    };

    Derived* CreateRandomDerivedHeap()
    {
        return dataGeneration.Generator<Derived>().RandomHeap<int, std::string>();
    }
};

IDManagerTestsFixture::Base::~Base() = default;

namespace Inscription
{
    template<>
    struct TableData<::IDManagerTestsFixture::Base, BinaryArchive> :
        TableDataBase<::IDManagerTestsFixture::Base, BinaryArchive>
    {
        int baseValue;
    };

    template<>
    class Scribe<::IDManagerTestsFixture::Base, BinaryArchive> final :
        public TableScribe<::IDManagerTestsFixture::Base, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table();
        };
    };

    template<>
    struct TableData<::IDManagerTestsFixture::Derived, BinaryArchive> :
        TableDataBase<::IDManagerTestsFixture::Derived, BinaryArchive>
    {
        Base<::IDManagerTestsFixture::Base> base;
        std::string derivedValue;
    };

    template<>
    class Scribe<::IDManagerTestsFixture::Derived, BinaryArchive> final :
        public TableScribe<::IDManagerTestsFixture::Derived, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table();
        };
    public:
        static TypeHandle PrincipleTypeHandle(const ArchiveT& archive);
    };
}

SCENARIO_METHOD(IDManagerTestsFixture, "IDManager")
{
    GIVEN("id manager stored integers")
    {
        using TestedObject = IDManager<int, int>;
        TestedObject idManager;

        WHEN("adding three entries")
        {
            idManager.Add(1, 0);
            idManager.Add(2, 1);
            idManager.Add(3, 2);

            THEN("saving and loading gives equal object as starting")
            {
                {
                    auto outputArchive = ::Inscription::OutputBinaryArchive
                    (
                        "Test.dat", 
                        "testing", 
                        1
                    );
                    outputArchive(idManager);
                }

                TestedObject loadedIDManager;

                {
                    auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "testing");
                    inputArchive(loadedIDManager);
                }

                REQUIRE(!loadedIDManager.IsEmpty());
                REQUIRE(loadedIDManager == idManager);
            }
        }
    }

    GIVEN("id manager stored unique ptrs")
    {
        using BasePtr = std::unique_ptr<Base>;
        using TestedObject = IDManager<int, BasePtr>;
        TestedObject idManager;

        WHEN("adding three entries")
        {
            idManager.Add(1, BasePtr(CreateRandomDerivedHeap()));
            idManager.Add(2, BasePtr(CreateRandomDerivedHeap()));
            idManager.Add(3, BasePtr(CreateRandomDerivedHeap()));

            THEN("saving and loading gives equal object as starting")
            {
                {
                    auto outputArchive = ::Inscription::OutputBinaryArchive
                    (
                        "Test.dat", 
                        "testing", 
                        1
                    );
                    outputArchive.RegisterType<Base>();
                    outputArchive.RegisterType<Derived>();
                    outputArchive(idManager);
                }

                TestedObject loadedIDManager;

                {
                    auto inputArchive = ::Inscription::InputBinaryArchive("Test.dat", "testing");
                    inputArchive.RegisterType<Base>();
                    inputArchive.RegisterType<Derived>();
                    inputArchive(loadedIDManager);
                }

                REQUIRE(!loadedIDManager.IsEmpty());
                for (size_t i = 1; i < loadedIDManager.Size() + 1; ++i)
                {
                    auto startingFound = dynamic_cast<Derived*>(idManager.Find(i)->get());
                    auto loadedFound = dynamic_cast<Derived*>(loadedIDManager.Find(i)->get());
                    REQUIRE(startingFound->baseValue == loadedFound->baseValue);
                    REQUIRE(startingFound->derivedValue == loadedFound->derivedValue);
                }
            }
        }
    }
}

namespace Inscription
{
    Scribe<::IDManagerTestsFixture::Base, BinaryArchive>::Table::Table()
    {
        MergeDataLinks
        ({
            DataLink::Auto(&ObjectT::baseValue, &DataT::baseValue) }
        );
    }

    Scribe<::IDManagerTestsFixture::Derived, BinaryArchive>::Table::Table()
    {
        MergeDataLinks
        ({
            DataLink::Base(data.base),
            DataLink::Auto(&ObjectT::derivedValue, &DataT::derivedValue) }
        );
    }

    TypeHandle Scribe<::IDManagerTestsFixture::Derived, BinaryArchive>::PrincipleTypeHandle(const ArchiveT& archive)
    {
        return "Derived";
    }
}